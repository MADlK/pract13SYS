// 13SYS.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>
#include <Conio.h>
using namespace std;

struct Boss
{
    long health =9000000000;
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

//количество игроков
int playercount;
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



//поток босса
DWORD WINAPI BossThread(LPVOID B)
{
    Boss* bosss = (Boss*)B;
    long cdAttack = GetTickCount();
    long cdSpecialAttack = GetTickCount();
    while (bosss->health > 0)
    {
        /*если время с прошлой атаки + время ожидания
            больше чем текущее время, то можно бить обычной атакой*/
        if(cdAttack + bosss->attackCooldown > GetTickCount())
        {
            
            int playerid = getRandomPlayer(playercount);
            //активировать ивент по рандомному игроку
            

            /*ивент события что босс ударил по конкретному игроку
                в потоке игрока есть опрос произошел ли этот ивент,
                если да, то идет проверка уклонился ли игрок или нет
                , если игрок не уклонился, то по нему проходит эффективный урон
            int dmg = bosss->damage - players[playerid].defense;
            в конце ивент деактивировать*/
            
            
            
        }
        /*если время с прошлой атаки + время ожидания
            больше чем текущее время, то можно бить спец. атакой*/
        if (cdSpecialAttack + bosss->specialCooldown > GetTickCount())
        {
            //активировать ивент


            /*в потоках игроков идет опрос сделался ли этот ивент,
                ивент должен активироваться сразу у всех игроковб,
                затем идет проверка уклонился ли игрок, если нет,
                то проходит эффективный урон
            int specDmg = bosss->specialDamage - защита игрока
            в конце ивент деактивировать*/
        }
         //ударил ли ктото по боссу (использовать мьютекс, возможно семафор)
        if (true)
        {
            bosss->health - (players[0].damage - bosss->resist);
        }
        //ударил ли ктото по боссу спец. атакой (использовать мьютекс, возможно семафор)
        if (true)
        {
            bosss->health - (players[0].damage - bosss->resist);
        }
    }

}

//поток игроков
DWORD WINAPI PlayerThread(LPVOID P)
{
    Player* playr = (Player*)P;
    long cdAttack = GetTickCount();
    long cdSpecialAttack = GetTickCount();
    while (playr->health > 0)
    {
        //опрос на получения урона обычной атаки босса
        if (true)
        {
            if (dodge())
            {
                cout << playr->name << "уклонился от атаки босса" << endl;
            }
            else
            {
                playr->health - (boss.damage - playr->defense);
                cout << playr->name << "получил урон от босса:" << (boss.damage - playr->defense) <<endl;
            }
        }



        //опрос на получения урона спец. атаки босса
        if (true)
        {
            if (dodge())
            {
                cout << playr->name << "уклонился от атаки босса" << endl;
            }
            else
            {
                playr->health - (boss.specialDamage - playr->defense);
                cout << playr->name << "получил урон от спец. атаки босса:" << (boss.specialDamage - playr->defense) << endl;
            }
        }
        //кд для обычных атак
        if(cdAttack + playr->attackCooldown > GetTickCount())
        {

        }
    }
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
    cout<<"Кол-во игроков"<<playercount << endl;
    //выбрал имена
    SelectNameForPlayers();

    


    
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
