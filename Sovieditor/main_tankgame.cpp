#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

sf::RenderWindow window(sf::VideoMode(800, 600), "Tankgame");// TODO Local

/*void Graphics::draw(std::vector<Line> line) {
    //line_size = s;
    getLines(line);
    rect2.setFillColor(sf::Color::Blue);
    rect2.setOutlineThickness(0);

    for(int i = 0;i<point.size();i++) {
        rect2.setPosition(point[i].x1 * tileSize, point[i].y1 * tileSize);
        rect2.setSize(sf::Vector2f(tileSize, (point[i].y2 - point[i].y1) * tileSize));
        window.draw(rect2);
    }
    point.resize(0);
}*/

struct bunker_grid {
    int x, y, bo, ch;
};

struct beam {
    int x, y;
};

struct enemy {
    int x, y, level, shooting_time;
};

class bunker { // TODO Bessere Klasse
public:
    int x, y, tilesize;
    int m[2][3] = {{1, 1, 1}, {1, 0, 1}};
    std::vector<bunker_grid> mapp;
    sf::RenderWindow* w;
    int blast_area[3][5] = {
        1, 1, 1, 1, 1,
        0, 1, 1, 1, 0,
        0, 1, 1, 1, 0
    };
    int point_of_destruction_x;
    int point_of_destruction_y;

    bunker() {
        tilesize = 20;
        point_of_destruction_x = 2;
        point_of_destruction_y = 0;
        //stuffing(x, y, m, mapp);
    }

    void update() {
        stuffing(x, y, m, mapp);
    }

    void stuffing(int x, int y, int m[2][3], std::vector<bunker_grid> mapp) {
        for(int i = 0;i<2;i++) {
            for(int j = 0;j<3;j++) {
                if(m[i][j] == 1) {
                    fill_arr(x+(tilesize*j), y+(tilesize*i), mapp);
                }
            }
        }
    }

    void destruction(int x, int y) {
        std::vector<beam> des;

        int x1, y1;

        for(int i = 0;i<3;i++) {
            for(int j = 0;j<5;j++) {
                if(blast_area[i][j] == 1) {
                    x1 = x + ((j - point_of_destruction_x) * 3);
                    y1 = y + ((i - point_of_destruction_y) * 3);
                    destr(x1, y1, &des);
                }
            }
        }

        for(int i = 0;i<des.size();i++) {
            for(int j = 0;j<mapp.size();j++) {
                if(mapp[j].x == des[i].x && mapp[j].y == des[i].y) {
                    mapp.erase(mapp.begin()+j);
                }
            }
		}
    }

    void destr(int x, int y, std::vector<beam> *d) {
        beam destruction_beam;

        for(int i = 0;i<3;i++) {
            for(int j = 0;j<3;j++) {
                destruction_beam.x = x+j;
                destruction_beam.y = y+i;
                (*d).push_back(destruction_beam);
            }
        }
    }

    void fill_arr(int x, int y, std::vector<bunker_grid> ma) {
        bunker_grid b;

        for(int i = 0;i<tilesize;i++) {
            for(int j = 0;j<tilesize;j++) {
                b.x = j + x;
                b.y = i + y;
                b.bo = 0;
                b.ch = 0;

                mapp.push_back(b);
            }
        }
    }

    void draw_bunker() {
        sf::RectangleShape r;
        for(int i = 0;i<mapp.size();i++) {
            r.setSize(sf::Vector2f(1, 1));
            r.setOutlineThickness(0);
            r.setFillColor(sf::Color::Blue);
            r.setPosition(mapp[i].x, mapp[i].y);
            window.draw(r);
        }
    }

    //Grenzen Funktion
    void check_bunker_shoot(std::vector<beam>* b, std::vector<beam>* my_b) { //Pointer hat problem gelöst
        for(int i = 0;i<mapp.size();i++) {
            for(int j = 0;j<(*b).size();j++) {
                if(mapp[i].x > (*b)[j].x && mapp[i].x < (*b)[j].x+5 && mapp[i].y > (*b)[j].y && mapp[i].y < (*b)[j].y+10) {
                    (*b).erase((*b).begin()+j);
                    destruction(mapp[i].x, mapp[i].y);
                }
            }
            for(int j = 0;j<(*my_b).size();j++) {
                if(mapp[i].x > (*my_b)[j].x && mapp[i].x < (*my_b)[j].x+5 && mapp[i].y > (*my_b)[j].y && mapp[i].y < (*my_b)[j].y+10) {
                    (*my_b).erase((*my_b).begin()+j);
                    destruction(mapp[i].x, mapp[i].y-4);
                }
            }
        }
    }

};

int main() {
    window.setFramerateLimit(60);

    std::vector<beam> beams;
    std::vector<beam> enemy_beams;
    std::vector<enemy> enemys;

    bunker b;
    b.x = 70;
    b.y = 500;


    bunker bunker_array[4];

    for(int i = 0;i<4;i++) {
        bunker_array[i] = b;
        bunker_array[i].update();
        b.x += 100;
    }


    /*for(int g = 0;g<b.mapp.size();g++) {
        std::cout << b.mapp[g].x << "fdgg" << b.mapp[g].y << "dff";
    }*/


    enemy loop_enemy;

    int shooting_time_chooser = 0;

    for(int i = 0;i<3;i++) {
        for(int j = 0;j<10;j++) {
            if(shooting_time_chooser > 2) {
                shooting_time_chooser = 0;
            }
            loop_enemy.x = (j * 35) + 35;
            loop_enemy.y = (i * 35) + 35;
            loop_enemy.level = 0;
            loop_enemy.shooting_time = shooting_time_chooser;
            shooting_time_chooser++;
            enemys.push_back(loop_enemy);
        }
    }

    sf::Color c(178, 0, 0);

     sf::RectangleShape bottom;
     bottom.setSize(sf::Vector2f(800, 20));
     bottom.setOutlineThickness(0);
     bottom.setFillColor(c);
     bottom.setPosition(0, 580);

     c.r = 0;
     c.b = 203;

     int playerX = 0;
     int playerY = 555;
     int gesw = 2;

     sf::RectangleShape player;
     player.setSize(sf::Vector2f(25, 25));
     player.setOutlineThickness(0);
     player.setFillColor(c);
     player.setPosition(playerX, playerY);

     c.b = 0;
     c.r = 103;

     sf::RectangleShape rect_beam;
     rect_beam.setSize(sf::Vector2f(5, 10));
     rect_beam.setOutlineThickness(0);
     rect_beam.setFillColor(c);
     rect_beam.setPosition(playerX, playerY);

     c.b = 0;
     c.r = 103;
     c.g = 156;

     int enemy_movement = 10;
     bool enemy_movement_right = true;
     bool enemy_movement_down = false;

     sf::RectangleShape rect_enemy;
     rect_enemy.setSize(sf::Vector2f(20, 20));
     rect_enemy.setOutlineThickness(0);
     rect_enemy.setFillColor(c);
     rect_enemy.setPosition(playerX, playerY);

     c.b = 0;
     c.r = 0;
     c.g = 234;

     sf::RectangleShape rect_enemy_beam;
     rect_enemy_beam.setSize(sf::Vector2f(5, 10));
     rect_enemy_beam.setOutlineThickness(0);
     rect_enemy_beam.setFillColor(c);
     rect_enemy_beam.setPosition(playerX, playerY);

     sf::Font font;
    if (!font.loadFromFile("Arial.ttf"))
        return EXIT_FAILURE;
    sf::Text text("Game Over!", font, 50);
    text.setFillColor(sf::Color::Blue);
    text.setStyle(sf::Text::Regular);
    text.setPosition(sf::Vector2f(200, 200));

    sf::Text Score(std::to_string(3), font, 30);
    Score.setFillColor(sf::Color::White);
    Score.setStyle(sf::Text::Regular);
    Score.setPosition(sf::Vector2f(20, 0));

     auto ka1 = std::chrono::steady_clock::now();
     auto ka2 = std::chrono::steady_clock::now();
     auto ka3 = std::chrono::steady_clock::now();
     auto ka4 = std::chrono::steady_clock::now();
     std::chrono::duration<double> enemy_shooting_time;

     int ran = 0;
     int life = 3;

     int lint = 0;
     int rint = 0;
     int upint = 0;
     int dint = 0;

     int ranMax = 50;
     int old_ranMax = 0;

     bool loose = false;

     while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            playerX-=gesw;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            playerX+=gesw;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                auto ka2 = std::chrono::steady_clock::now();
                 std::chrono::duration<double> ela = ka2 - ka1;
                if(ela.count() >= 0.3 && !loose) {
                    //playerY+=gesw;
                    beam b;
                    b.x = playerX + 12;
                    b.y = playerY;

                    beams.push_back(b);

                    ka1 = std::chrono::steady_clock::now();
                }
            }
            /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || (lint > 0)) {
                playerX-=gesw;
                if(lint > 0) {
                    lint--;
                }else {
                    lint = 4;
                    rint = 0;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || (rint > 0)) {
                playerX+=gesw;
                if(rint > 0) {
                    rint--;
                }else {
                    rint = 4;
                    lint = 0;
                }
            }*/
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || upint > 0) {
                //playerY-=gesw;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || dint > 0) {
                //playerY+=gesw;
            }
        }

        player.setPosition(playerX, playerY);

        window.clear();

        ka2 = std::chrono::steady_clock::now();
        enemy_shooting_time = ka2 - ka3;

        if(enemy_shooting_time.count() >= 0.2) {

            if(enemy_movement == 0) {
                enemy_movement_down = true;
                enemy_movement = 10;
                enemy_movement_right = !enemy_movement_right;
            }else {
                enemy_movement-=1;
            }

            for(int i = 0;i<enemys.size();i++) {
                if (enemy_movement_down) {
                    enemys[i].y += 10;
                }else if(enemy_movement_right) {
                    enemys[i].x += 4;
                }else {
                    enemys[i].x -= 4;
                }
            }

            enemy_movement_down = false;
            ka3 = std::chrono::steady_clock::now();
        }


        ka2 = std::chrono::steady_clock::now();
        enemy_shooting_time = ka2 - ka4;
        if(enemy_shooting_time.count() >= 0.1) {
            //std::srand(static_cast<unsigned int>(std::time(nullptr)));
            ran = 0 + ( std::rand() % ( ranMax ) ); //60 recht gut

            for(int i = 0;i<enemys.size();i++) {
                if(i == ran) {
                    beam b;
                    b.x = enemys[ran].x;
                    b.y = enemys[ran].y;
                    enemy_beams.push_back(b);
                }
            }
            ka4 = std::chrono::steady_clock::now();
            if((30 - enemys.size()) % 10 == 0 && (30-enemys.size()) != old_ranMax) {
                ranMax -= 30 -enemys.size();
                //std::cout << ranMax << "retrte";
                old_ranMax = (30 - enemys.size());
            }
        }

        for(int i = 0;i<enemy_beams.size();i++) {
            enemy_beams[i].y+=4;
            if(enemy_beams[i].y == 600) {
                enemy_beams.erase(enemy_beams.begin() + i);
            }else if((enemy_beams[i].y + 10) >= playerY && (enemy_beams[i].y + 10) <= 580 && enemy_beams[i].x >= playerX && enemy_beams[i].x <= (playerX + 25)) {
                enemy_beams.erase(enemy_beams.begin() + i);
                life--;
            }else {
                rect_enemy_beam.setPosition(enemy_beams[i].x, enemy_beams[i].y);
                window.draw(rect_enemy_beam);
            }

        }

        for(int i = 0;i<beams.size();i++) {
            for(int k = 0;k<enemys.size();k++) {
                if(beams[i].x > enemys[k].x && beams[i].x < (enemys[k].x+20) && beams[i].y > enemys[k].y && beams[i].y < (enemys[k].y+20)) {
                    beams.erase(beams.begin() + i);
                    enemys.erase(enemys.begin() + k);
                }else if(beams[i].x+5 > enemys[k].x && beams[i].x+5 < (enemys[k].x+20) && beams[i].y > enemys[k].y && beams[i].y < (enemys[k].y+20)) {
                    beams.erase(beams.begin() + i);
                    enemys.erase(enemys.begin() + k);
                }
            }
        }

        for(int i = 0;i<beams.size();i++) {
            beams[i].y-=6;
            if(beams[i].y == 0) {
                beams.erase(beams.begin() + i);
            }else {
                rect_beam.setPosition(beams[i].x, beams[i].y);
                window.draw(rect_beam);
            }
        }

        for(int i = 0;i<enemys.size();i++) {
            rect_enemy.setPosition(enemys[i].x, enemys[i].y);
            window.draw(rect_enemy);
        }

        Score.setString("Life:" + std::to_string(life));

        if(life <= 0) {
            window.draw(text);
            loose = true;
        }

        if(enemys.size() == 0) {
            text.setString("Game Won!");
            window.clear();
            window.draw(text);
        }

        for(int i = 0;i<4;i++) {
            bunker_array[i].check_bunker_shoot(&enemy_beams, &beams);
            bunker_array[i].draw_bunker();
        }

        window.draw(Score);
        window.draw(bottom);
        window.draw(player);
        window.display();
    }

	return 0;
}
