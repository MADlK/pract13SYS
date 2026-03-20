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
    long health = 500000;
    int damage = 12000;
    int specialDamage = 30000;
    int specialCooldown = 5000;
    int attackCooldown = 2000;
    int defense = 20;
    int dodgechance = 15;
    char name[64];

};

struct Top
{
	char Name[64];
	int totaldamage;
};

HANDLE dmgSemafor;
HANDLE PlayersDied;
HANDLE BOSSSPECDMG;
HANDLE PlayerWaitBossSpecDMG;
HANDLE endgame;


//количество игроков
int playercount;
//количество живых игроков
int playerCountLife;
Top topthree[3];


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
        cout << players[playerid].name << "Уклонился от атаки" << endl;
    }
    else
    {
        
        double EndSpDmg = (boss.damage / 100) * (100 - players[playerid].defense);
        players[playerid].health -=  EndSpDmg;
        cout << players[playerid].name << "получил урон от босса" << EndSpDmg << endl;
    }
}
//метод нанесения спуц. атак всем живым игрокам
void SPECIALDAMAGEBOSS(Player p)
{
    
    
        if (dodge())
        {
            cout << p.name << " уклонился от спец. атаки босса" << endl;
        }
        else
        {
            double SpDmg = boss.damage * (1 - 0.05 * (playercount - 1));
            double EndSpDmg = (SpDmg / 100) * (100 - p.defense);
            p.health -= EndSpDmg;
            cout << p.name << " получил урон спец. атаки босса"<< EndSpDmg<< endl;
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
            //SPECIALDAMAGEBOSS();
            PulseEvent(BOSSSPECDMG);
            cdSpecialAttack = GetTickCount();
        }
        
    }
    
    return 0;
}
DWORD WINAPI ENDGAME(LPVOID lp) 
{
    WaitForSingleObject(PlayersDied,INFINITE);
    


    if (playerCountLife < 0)
    {
        cout << "Boss winner" << endl;
        cout << "ХП босса" << boss.health << endl;
    }

    
    else
        cout<<"players winner"<< endl;


    for (int i = 0; i < 3; i++)
    {
        cout <<"Игрок "<<topthree[i].Name <<" нанес " << topthree[i].totaldamage<<" урона"   <<endl;
    }
	ResetEvent(PlayersDied);

    return 0;
}
DWORD WINAPI PlayerWaitBossSpecDMGThread(LPVOID P)
{
    //в отдельный поток
    
    Player* playr = (Player*)P;
    while (playr->health>0)
    {
        DWORD waitResult = WaitForSingleObject(BOSSSPECDMG, INFINITE);

        if (playr->health > 0)
        {
            if (waitResult == WAIT_OBJECT_0)
            {

                SPECIALDAMAGEBOSS(*playr);
                ResetEvent(BOSSSPECDMG);
            }
        }
        
    }

    return 0;
}
//поток игроков
DWORD WINAPI PlayerThread(LPVOID P)
{
    Player* playr = (Player*)P;
    Top dmg;
    dmg.totaldamage = 0;
    strcpy_s(dmg.Name, playr->name);

    
    
    HANDLE PlayerWaitBossSpecDMG = CreateThread(NULL, 0, PlayerWaitBossSpecDMGThread, playr, 0, NULL);
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
                dmg.totaldamage += EndSpDmg;
                cout << "босс получил урон " << EndSpDmg << " от " << playr->name << endl;
                cdAttack = GetTickCount();
                cout << boss.health << endl;
            }
            else if (cdSpecialAttack + playr->specialCooldown < GetTickCount())
            {
                double EndSpDmg = playr->specialDamage * (100 - boss.resist) / 100;
                boss.health -= EndSpDmg;
                dmg.totaldamage += EndSpDmg;
                cout << "босс получил урон спец. атакой " << EndSpDmg << " от " << playr->name << endl;
                cdSpecialAttack = GetTickCount();
                cout << boss.health << endl;
            }
        }

        
        ReleaseSemaphore(dmgSemafor,1,NULL);
    }
    cout <<playr->name <<" - игрок умер и больше не должен получать урон от спец атак босса--------------------------------------------" << endl;
    CloseHandle(PlayerWaitBossSpecDMG);

    if (playerCountLife == 3)
    {
        topthree[2] = dmg;
    }
    else if (playerCountLife == 2)
    {
        topthree[1] = dmg;
    }
    else if (playerCountLife == 1)
    {
        topthree[0] = dmg;
    }


    if (playerCountLife >= 1)
    {
        playerCountLife--;
        if(playerCountLife == 0)
            SetEvent(PlayersDied);
    }
        
    cout << playerCountLife << " живых игроков" << endl;
    return 0;
}


//придумать как хранить топ3 по урону, есть структура топ 3,
// при каждом нанесении урона игроком, сравнивать его урон с топ 3
// и если он больше, то вставлять его в топ 3, а тот кто был на 3 месте удалять из топ 3
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
    BOSSSPECDMG = CreateEvent(NULL,TRUE,FALSE,NULL);
    dmgSemafor = CreateSemaphore(NULL,1,1,NULL);
    HANDLE threads[11];
    for (int i = 0; i < playercount; i++)
    {
       
        threads[i] = CreateThread(NULL, 0, PlayerThread, &players[i], 0, NULL);
    }
    cout<<"Игроки зашли к боссу" << endl;
    HANDLE bosssss = CreateThread(NULL,0,BossThread,NULL,0,NULL);
    

    endgame = CreateThread(NULL, 0, ENDGAME, NULL, 0, NULL);

    WaitForMultipleObjects(playercount,threads,TRUE,INFINITE);
    WaitForSingleObject(bosssss, INFINITE);
    
    
    
    cout<<playerCountLife<< endl;
    for (int i = 0; i < playercount; i++)
    {
         
        CloseHandle(threads[i]);
    }
    CloseHandle(bosssss);
    CloseHandle(BOSSSPECDMG);
    CloseHandle(PlayersDied);
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
