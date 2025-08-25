#include <SFML/Graphics.hpp>
#include <sstream>
#include "position.h"
#include "bitboard.h"
#include "search.h"

static const int TILE = 80;

int main(){
    sf::RenderWindow window(sf::VideoMode(TILE*8, TILE*8+60), "Chess Engine UI");

    bitboards_init();
    PSQT::init();
    StateInfo st{};
    Position pos;
    pos.set("startpos", WHITE, &st);
    Searcher searcher;
    int depth = 4;
    SearchResult res = searcher.search(pos, depth);

    while(window.isOpen()){
        sf::Event e; while(window.pollEvent(e)){ if(e.type==sf::Event::Closed) window.close(); }
        window.clear(sf::Color(30,30,30));
        // Draw board
        for(int r=0;r<8;++r){ for(int f=0; f<8; ++f){ sf::RectangleShape rect(sf::Vector2f(TILE, TILE)); rect.setPosition(f*TILE, (7-r)*TILE); bool light=((r+f)&1)==0; rect.setFillColor(light?sf::Color(240,217,181):sf::Color(181,136,99)); window.draw(rect);} }
        // HUD
        sf::Font font; // load default font if available in system; otherwise skip text
        if(font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")){
            sf::Text t; t.setFont(font); t.setCharacterSize(20); t.setFillColor(sf::Color::White);
            std::ostringstream oss; oss << "Eval: " << res.eval << "  Depth: " << res.depth;
            t.setString(oss.str()); t.setPosition(10, TILE*8 + 10); window.draw(t);
        }
        window.display();
    }
    return 0;
}

