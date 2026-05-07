#include <string>
#include <vector>
#include "raylib.h"



//provisorisch später durch echte liste ersetzt
struct Inventar_Items
{   
    std::string name;
    int anzahl;
};
enum direction {
    Up,
    Down,
    Right,
    Left
};
class player {
    private:
        int posX;
        int posY;
        float speed = 10;
        std::vector<Inventar_Items> Inventar;
        std::string name;



    public:

        void Set_position(int posx, int posy) {
            posX = posx;
            posY = posy;
        };
        void Move(direction dir, float deltaTime) {
            switch(dir) {
                case Right: posX += speed * deltaTime; break;
                case Left:  posX -= speed * deltaTime; break;
                case Up:    posY -= speed * deltaTime; break;
                case Down:  posY += speed * deltaTime; break;
            }
        }
        Vector2 Get_position() { 
            Vector2 position = {posX, posY};
            return position;
        };
        void Change_Name(std::string Name) { name = Name; };
        std::string Get_Name() { return name; };

        Vector2 Get_position() const { return {(float)posX, (float)posY}; }
        std::string Get_Name() const { return name; }

};

void loadPlayer(player& p);
void savePlayer(const player& p);