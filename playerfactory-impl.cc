module player;

using namespace std;

unique_ptr<Player> PlayerFactory::createPlayer(
        const string& type, Colour c) {
    if (type == "human")
        return make_unique<HumanPlayer>(c);
    if (type == "computer1") return make_unique<Level1Player>(c);
    if (type == "computer2") return make_unique<Level2Player>(c);
    if (type == "computer3") return make_unique<Level3Player>(c);
    if (type == "computer4") return make_unique<Level4Player>(c);
    return nullptr;
}
