#include <iostream>
#include <Windows.h>
#include <Conio.h>
using namespace std;

struct Boss
{
    long health = 9000000;
    int resist = 44;
    int damage = 73843;
    int specialDamage = 150000;
    int attackCooldown = 5000;
    int specialCooldown = 10000;
    
};
struct Player
{
    long health = 70000;
    int damage = 12000;
    int specialDamage = 30000;
    int specialCooldown = 5000;
    int attackCooldown = 2000;
    int defense = 20;
    int dodgechance = 15;
    char name[64];

};


HANDLE dmgSemafor;
HANDLE PlayersDied;
//HANDLE dmgSemafor;

//количество игроков
int playercount;
//количество живых игроков
int playerCountLife;

//массив игроков
Player players[10];
//босс
Boss boss;
//функция для указания количества игроков
int HowManyPlayers()
{
    int howplayers;
    while (true)
    {
        cout << "Выберите количество игроков в группе" << endl;
        cin >> howplayers;
        if (howplayers > 10)
            cout << "В группе не может быть больше 10 игроков" << endl;
        else
            break;
    }
    return howplayers;
}
//метод для выбора имен игрокам
void SelectNameForPlayers()
{
    for (int i = 0; i < playercount; i++)
    {
        cout << endl;
        cout << "Введите имя игрока" << endl;
        cin>> players[i].name;
        cout << "имя игрока "<< players[i].name << endl;
        
    }
    
}
//поулчение рандомного игрока для обычных атак босса
DWORD getRandomPlayer( int max) {
    return 0 + rand() % (max - 0 + 1);
}
//получение числа , чтобы понять сработал ли шанс уклонения
DWORD getRandom(int minMs, int maxMs) {
    return minMs + rand() % (maxMs - minMs + 1);
}
//сработало ли уклонение
bool dodge()
{
    if (getRandom(1, 100) < 16)
        return true;
    return false;

}
//метод нанесения обычных атак рандомному живому игроку
void BossDMG()
{
    int playerid;
    do
    {
        playerid = getRandomPlayer(playercount);
    } while (players[playerid].health <= 0);

    if (dodge())
    {
        cout << players[playerid].name << "Уклонрился от атаки" << endl;
    }
    else
    {
        
        double EndSpDmg = (boss.damage / 100) * (100 - players[playerid].defense);
        players[playerid].health -=  EndSpDmg;
        cout << players[playerid].name << "получил урон от босса" << EndSpDmg << endl;
    }
}

//метод нанесения спуц. атак всем живым игрокам
void SPECIALDAMAGEBOSS()
{
    
    for (int i = 0; i < playercount; i++)
    {
        if (dodge())
        {
            cout << players[i].name << "уклонился от спец. атаки босса" << endl;
        }
        else
        {
            double SpDmg = boss.damage * (1 - 0.05 * (playercount - 1));
            double EndSpDmg = (SpDmg / 100) * (100 - players[i].defense);
            players[i].health -= EndSpDmg;
            cout << players[i].name << "получил урон спец. атаки босса"<< EndSpDmg<< endl;
        }
        
    }
    
}



//поток босса
DWORD WINAPI BossThread(LPVOID B)
{
    
    long cdAttack = GetTickCount();
    long cdSpecialAttack = GetTickCount();
    
    while (boss.health > 0 && playerCountLife>0)
    {
        
        /*если время с прошлой атаки + время ожидания
            больше чем текущее время, то можно бить обычной атакой*/
        if(cdAttack + boss.attackCooldown < GetTickCount())
        {

            BossDMG();
            cdAttack = GetTickCount();
        }
        /*если время с прошлой атаки + время ожидания
            больше чем текущее время, то можно бить спец. атакой*/
        else if (cdSpecialAttack + boss.specialCooldown < GetTickCount())
        {
            SPECIALDAMAGEBOSS();
            cdSpecialAttack = GetTickCount();
        }
        
    }
    
    return 0;
}














DWORD WINAPI ENDGAME(LPVOID lp) 
{
    WaitForSingleObject(PlayersDied,INFINITE);
    cout<<"типо топ 3 игрока" << endl;


    if (playerCountLife < 0)
        cout << "Boss winner" << endl;
    else
        cout<<"players winner"<< endl;
    

    return 0;
}



//поток игроков
DWORD WINAPI PlayerThread(LPVOID P)
{
    Player* playr = (Player*)P;
    long cdAttack = GetTickCount();
    long cdSpecialAttack = GetTickCount();
    while (playr->health > 0)
    {
        WaitForSingleObject(dmgSemafor, INFINITE);
        if (playr->health > 0)
        {
            if (cdAttack + playr->attackCooldown < GetTickCount())
            {
                double EndSpDmg = playr->damage * (100 - boss.resist) / 100;
                boss.health -= EndSpDmg;
                cout << "босс получил урон " << EndSpDmg << " от " << playr->name << endl;
                cdAttack = GetTickCount();
                cout << boss.health << endl;
            }
            else if (cdSpecialAttack + playr->specialCooldown < GetTickCount())
            {
                double EndSpDmg = playr->specialDamage * (100 - boss.resist) / 100;
                boss.health -= EndSpDmg;
                cout << "босс получил урон спец. атакой " << EndSpDmg << " от " << playr->name << endl;
                cdSpecialAttack = GetTickCount();
                cout << boss.health << endl;
            }
        }
        //опрос на получения урона обычной атаки босса
        



       
        
        ReleaseSemaphore(dmgSemafor,1,NULL);
    }
    cout<< playr->name <<" умер" << endl;
   

    if (playerCountLife >= 1)
    {
        playerCountLife--;
        if(playerCountLife == 0)
            SetEvent(PlayersDied);
    }
        
    cout << playerCountLife << " живых игроков" << endl;
    return 0;
}


//void BossInit()
//{
//    if (!CreateThread())
//    {
//
//    }
//}



int main()
{
    srand(0);
    setlocale(0,"ru");
    //назначил количество игроков
    playercount = HowManyPlayers();
    playerCountLife = playercount;
    cout<<"Кол-во игроков"<<playercount << endl;
    //выбрал имена
    SelectNameForPlayers();
    PlayersDied = CreateEvent(NULL,FALSE,FALSE,NULL);
    dmgSemafor = CreateSemaphore(NULL,1,1,NULL);
    HANDLE threads[11];
    for (int i = 0; i < playercount; i++)
    {
        threads[i] = CreateThread(NULL, 0, PlayerThread, &players[i], 0, NULL);
    }
    cout<<"Игроки зашли к боссу" << endl;
    HANDLE bosssss = CreateThread(NULL,0,BossThread,NULL,0,NULL);
    

    WaitForMultipleObjects(playercount,threads,TRUE,INFINITE);
    WaitForSingleObject(bosssss, INFINITE);
    cout<<"Игра окончена ктото победил" << endl;
    cout<<boss.health << endl;
    cout<<playerCountLife<< endl;
    for (int i = 0; i < playercount; i++)
    {
        CloseHandle(threads[i]);
    }
    CloseHandle(bosssss);
    CloseHandle(dmgSemafor);
    
    


    
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
