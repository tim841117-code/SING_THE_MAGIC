#include "MyPG.h"
#include "MyGameMain.h"

#include <deque>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <mmsystem.h>   // timeGetTime()
#include <digitalv.h>
#pragma comment(lib, "winmm.lib")


// ok lets gogogo ganbare!
namespace Game
{
    //============================================================
    // 1) Data Structs
    //============================================================
    struct MapData {
        int      arr[12][20];
        ML::Box2D chip[64];
    };

    enum class Angle {
        Down, Left, Up, Right, Non
    };

    struct Chara {
        int   x;
        int   y;
        Angle angle;
    };

    struct Bullet {
        float x, y;
        float vx, vy;
        float life;      
    };

    struct Enemy {
        int  x, y;
        bool alive;
    };

    struct Note {
        float x, y;      // （tile*32 + 16）
        int   kind;      // 0:Up 1:Down 2:Left 3:Right
        float life;      
    };


    //============================================================
    // 2) Resources
    //============================================================
    DG::Font::SP  font;

    DG::Image::SP imgMapChip;
    DG::Image::SP imgPlayer;
    DG::Image::SP imgFireball;
    DG::Image::SP imgMagicCircle;
    DG::Image::SP imgEnemy;

    DG::Image::SP imgNoteSheet;
    DG::Image::SP imgUIMagicBase;
    DG::Image::SP imgUIMagicCircle;
    DG::Image::SP imgUIMagicFireball;
    DG::Image::SP imgUIMagicEmpty;

    DG::Image::SP imgUISongList;

    DG::Image::SP imgUIScoreBase;
    static const char* seNotePath[4] = {
    "./data/sound/note_up.wav",    // kind 0
    "./data/sound/note_down.wav",  // kind 1
    "./data/sound/note_left.wav",  // kind 2
    "./data/sound/note_right.wav"  // kind 3
    };
    static const char* bgmPath = "./data/sound/bgm.wav";

    //============================================================
    // 3) Game State
    //============================================================
    MapData mapData;
    Chara   player;

    ML::Box2D playerImage[4];

    std::vector<Bullet> bullets;
    std::vector<Enemy>  enemies;

    // --- Fireball input buffer ---
    std::deque<Angle> noteBuf;
    bool canFireball = false;

    // --- Magic Circle ---
    bool circleReady = false;
    bool circleActive = false;
    int  circleX = 0;
    int  circleY = 0;

    std::deque<ML::Point> posBuf;

    // --- Step Notes ---
    ML::Box2D          noteSrc[4];
    std::vector<Note>  notes;
    ML::Point          lastStepTile = { 0,0 };
    int score = 0;


    static const int ENEMY_GOAL_X = 2;


    //============================================================
    // 4) DeltaTime / Timers (seconds)
    //============================================================
    float deltaTime = 1.0f / 60.0f;     // for

    float circleTimer = 0.0f;           // the
    const float CIRCLE_LIFE_FRAMES = 10.0f;   // delta

    const float NOTE_LIFE_FRAMES = 2.0f;      // time

    float enemySpawnTimer = 0.0f;       // to
    const float ENEMY_SPAWN_INTERVAL = 5.0f; // work

    float enemyMoveTimer = 0.0f;        // it
    const float ENEMY_MOVE_INTERVAL = 3.0f;   // need

    //--- UI ---
    static const int SCREEN_W = 640;
    static const int SCREEN_H = 450;
    static const int GAME_H = 12 * 32;   // 384

    static const int HUD_Y = GAME_H;              // 384
    static const int HUD_H = SCREEN_H - GAME_H;   // 66

    static const int COL_W = SCREEN_W / 3;        // 213
    static const int LEFT_X = 0;
    static const int MID_X = COL_W;               // 213
    static const int RIGHT_X = COL_W * 2;           // 426
    static const int RIGHT_W = SCREEN_W - RIGHT_X;  // 214
    //============================================================
    // 5) Prototypes
    //============================================================
    static void PushNote(Angle a);

    static void SpawnFireball();
    static void Bullets_UpDate();
    static void Bullets_Render();

    static void Enemies_Spawn2();
    static void Enemies_UpDate();
    static void Enemies_Render();

    static void SpawnStepNote(Angle movedDir);
    static void Notes_UpDate();
    static void Notes_Render();

    void  Map_UpData(MapData& md_);
    void  Map_Render(MapData& md_);
    void  Map_Initialize(MapData& md_);
    bool  Map_load(MapData& md_, int n_);

    void  Player_Initialize(Chara& c_, int x_, int y_, Angle a_);
    Angle Player_UpDate(Chara& c_);
    void  Player_Render(Chara& c_);

    bool  Map_CheckHit(MapData& md_, ML::Point p_);
    void  Map_ChangeClip(MapData& md_, ML::Point p_);

    Angle Angle4(XI::VGamePad& inp_);
    static void UI_Render();

    //============================================================
    // 6) Helpers
    //============================================================
    static void UpDateDeltaTime()
    {
        static DWORD prev = timeGetTime();
        DWORD now = timeGetTime();

        float dt = (now - prev) / 1000.0f;
        prev = now;

        // i dont know but it is for safe
        if (dt < 0.0f) dt = 0.0f;
        if (dt > 0.1f) dt = 0.1f; //  0.1s

        deltaTime = dt;
    }

    static void PlaySE_Note(int kind)
    {
        if (kind < 0 || kind >= 4) return;

        // SND_ASYNC：with se 
        // SND_FILENAME：find misic
        PlaySoundA(seNotePath[kind], NULL, SND_ASYNC | SND_FILENAME);
    }

    static bool IsPlayerInCircle3x3()
    {
        if (!circleActive) return false;
        return (player.x >= circleX && player.x <= circleX + 2 &&
            player.y >= circleY && player.y <= circleY + 2);
    }

    static bool CheckLoop3x3AndGetTopLeft(const std::deque<ML::Point>& b, int& outX, int& outY)
    {
        if (b.size() != 9) return false;
        if (b.front().x != b.back().x || b.front().y != b.back().y) return false;

        int minx = b[0].x, maxx = b[0].x;
        int miny = b[0].y, maxy = b[0].y;
        for (int i = 0; i < 8; ++i) {
            minx = (std::min)(minx, b[i].x);  maxx = (std::max)(maxx, b[i].x);
            miny = (std::min)(miny, b[i].y);  maxy = (std::max)(maxy, b[i].y);
        }

        if ((maxx - minx) != 2) return false;
        if ((maxy - miny) != 2) return false;

        for (int i = 0; i < 8; ++i) {
            int x = b[i].x, y = b[i].y;
            bool onEdge = (x == minx || x == maxx || y == miny || y == maxy);
            bool isCenter = (x == minx + 1 && y == miny + 1);
            if (!onEdge || isCenter) return false;
        }

        for (int i = 0; i < 8; ++i) {
            for (int j = i + 1; j < 8; ++j) {
                if (b[i].x == b[j].x && b[i].y == b[j].y) return false;
            }
        }

        outX = minx;
        outY = miny;
        return true;
    }
    static void PlayBGM()
    {
        mciSendStringA("close bgm", NULL, 0, NULL);

        char cmd[512];
        sprintf_s(cmd, "open \"%s\" type waveaudio alias bgm", bgmPath);
        mciSendStringA(cmd, NULL, 0, NULL);

        // 
        mciSendStringA("set bgm time format milliseconds", NULL, 0, NULL);

        mciSendStringA("play bgm from 0", NULL, 0, NULL);
    }
    static void BGM_KeepLoop()
    {
        char mode[64]{};
		// i dont know what is this for but i need it to work...
        if (mciSendStringA("status bgm mode", mode, (UINT)sizeof(mode), NULL) != 0) return;

        if (strcmp(mode, "stopped") == 0)
        {
			// restart
            mciSendStringA("play bgm from 0", NULL, 0, NULL);
        }
    }
    static void StopBGM()
    {
        mciSendStringA("stop bgm", NULL, 0, NULL);
        mciSendStringA("close bgm", NULL, 0, NULL);
    }

    
    //============================================================
    // 7) Note / Fireball / Bullet
    //============================================================
    static void PushNote(Angle a)
    {
        if (a == Angle::Non) return;
        if ((int)noteBuf.size() >= 3) noteBuf.pop_front();
        noteBuf.push_back(a);

        if (noteBuf.size() == 3)
        {
            Angle a0 = noteBuf[0];
            Angle a1 = noteBuf[1];
            Angle a2 = noteBuf[2];
            canFireball = (a0 != a1) && (a0 != a2) && (a1 != a2);
        }
        else {
            canFireball = false;
        }
    }

    static void SpawnFireball()
    {
        Bullet b{};
        b.x = player.x * 32.0f + 16.0f;
        b.y = player.y * 32.0f + 16.0f;

		// i dont know what is this but i need to chenge to seconds...
        const float spd = 360.0f;
        b.vx = 0; b.vy = 0;
        switch (player.angle) {
        case Angle::Left:  b.vx = -spd; break;
        case Angle::Right: b.vx = +spd; break;
        case Angle::Up:    b.vy = -spd; break;
        case Angle::Down:  b.vy = +spd; break;
        default:           b.vx = +spd; break;
        }

		b.life = 1.0f; // seconds
        bullets.push_back(b);
    }

    static void Bullets_UpDate()
    {
        for (auto& b : bullets) {
            b.x += b.vx * deltaTime;
            b.y += b.vy * deltaTime;
            b.life -= deltaTime;

            ML::Point p;
            p.x = (int)(b.x / 32.0f);
            p.y = (int)(b.y / 32.0f);

            if (Map_CheckHit(mapData, p)) b.life = 0;

            for (auto& e : enemies) {
                if (!e.alive) continue;
                if (e.x == p.x && e.y == p.y) {
                    e.alive = false;
                    b.life = 0;
                    score += 10;
                    break;
                }
            }
        }

        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                [](const Bullet& b) { return b.life <= 0.0f; }),
            bullets.end()
        );
    }

    static void Bullets_Render()
    {
        for (auto& b : bullets) {
            ML::Box2D dst((int)b.x - 16, (int)b.y - 16 - 12, 32, 32);
            ML::Box2D src(0, 0, 64, 64);
            imgFireball->Draw(dst, src);
        }
    }


    static int AngleToNoteKind(Angle a)
    {
        switch (a) {
        case Angle::Up:    return 0;
        case Angle::Down:  return 1;
        case Angle::Left:  return 2;
        case Angle::Right: return 3;
        default:           return -1;
        }
    }

    static void SpawnStepNote(Angle movedDir)
    {
        int k = AngleToNoteKind(movedDir);
        if (k < 0) return;

        Note n{};
        n.x = lastStepTile.x * 32.0f + 16.0f;
        n.y = lastStepTile.y * 32.0f + 16.0f;
        n.kind = k;
		n.life = NOTE_LIFE_FRAMES; // seconds
        notes.push_back(n);
        PlaySE_Note(n.kind);
    }

    static void Notes_UpDate()
    {
        for (auto& n : notes) n.life -= deltaTime;

        notes.erase(
            std::remove_if(notes.begin(), notes.end(),
                [](const Note& n) { return n.life <= 0.0f; }),
            notes.end()
        );
    }

    static void Notes_Render()
    {
        for (auto& n : notes) {
            ML::Box2D dst((int)n.x - 16, (int)n.y - 16 - 12, 32, 32);
            ML::Box2D src = noteSrc[n.kind];
            imgNoteSheet->Draw(dst, src);
        }
    }


    //============================================================
    // 8) Enemy
    //============================================================
    static int FindSpawnY_One(int x, int startY)
    {
        for (int k = 0; k < 12; ++k) {
            int y = (startY + k) % 12;
            if (!Map_CheckHit(mapData, ML::Point{ x, y })) return y;
        }
        return 0;
    }

    static void Enemies_Spawn2()
    {
        const int spawnX = 19;

        static int baseY = 1;
        int y1 = FindSpawnY_One(spawnX, baseY);
        int y2 = FindSpawnY_One(spawnX, (baseY + 6) % 12);
        if (y2 == y1) y2 = FindSpawnY_One(spawnX, (baseY + 1) % 12);

        baseY = (baseY + 1) % 12;

        enemies.push_back(Enemy{ spawnX, y1, true });
        enemies.push_back(Enemy{ spawnX, y2, true });
    }

    static void Enemies_UpDate()
    {
        enemyMoveTimer -= deltaTime;
        if (enemyMoveTimer <= 0.0f) {
            enemyMoveTimer += ENEMY_MOVE_INTERVAL;

            for (auto& e : enemies) {
                if (!e.alive) continue;

                int nx = e.x - 1;
                int ny = e.y;

                if (nx < 0) { e.alive = false; continue; }

                if (!Map_CheckHit(mapData, ML::Point{ nx, ny })) {
                    e.x = nx;
                }
            }
        }

        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& e) { return !e.alive; }),
            enemies.end()
        );
    }

    static void Enemies_Render()
    {
        for (auto& e : enemies) {
            if (!e.alive) continue;

            ML::Box2D dst(e.x * 32, e.y * 32 - 12, 32, 32);
            ML::Box2D src(0, 0, 32, 32);
            imgEnemy->Draw(dst, src);
        }
    }


    //============================================================
    // 9) Initialize / Finalize / UpDate / Render
    //============================================================
    void Initialize()
    {
        font = DG::Font::Create("MS Gothic", 16, 32);
        ge->dgi->EffectState().param.bgColor = ML::Color(1, 0, 1, 0);

        imgMapChip = DG::Image::Create("./data/image/MapChip01.bmp");
        imgPlayer = DG::Image::Create("./data/image/Chara01.png");
        imgFireball = DG::Image::Create("./data/image/Fireball.png");
        imgMagicCircle = DG::Image::Create("./data/image/MagicCircle96.png");
        imgEnemy = DG::Image::Create("./data/image/EnemySlime32.png");
        imgNoteSheet = DG::Image::Create("./data/image/note_spritesheet.png");
        imgUIMagicBase = DG::Image::Create("./data/image/ui_magic_base.png");
        imgUIMagicCircle = DG::Image::Create("./data/image/ui_magic_circle.png");
        imgUIMagicFireball = DG::Image::Create("./data/image/ui_magic_fireball.png");
        imgUIMagicEmpty = DG::Image::Create("./data/image/ui_magic_empty.png");

        imgUISongList = DG::Image::Create("./data/image/ui_songlist.png");

        imgUIScoreBase = DG::Image::Create("./data/image/ui_score_base.png");
        Map_Initialize(mapData);
        Map_load(mapData, 1);

        Player_Initialize(player, 5, 3, Angle::Down);

        for (int i = 0; i < 4; ++i) {
            playerImage[i] = ML::Box2D(i * 32, 0, 32, 32);
        }

        bullets.clear();
        noteBuf.clear();
        canFireball = false;

        posBuf.clear();
        posBuf.push_back(ML::Point{ player.x, player.y });

        circleReady = false;
        circleActive = false;
        circleTimer = 0.0f;

        for (int i = 0; i < 4; ++i) {
            noteSrc[i] = ML::Box2D(i * 32, 0, 32, 32);
        }

        notes.clear();
        lastStepTile = ML::Point{ player.x, player.y };

        enemies.clear();
        enemySpawnTimer = ENEMY_SPAWN_INTERVAL;
        enemyMoveTimer = ENEMY_MOVE_INTERVAL;
        score = 0;
        //  deltaTime 
        UpDateDeltaTime();
        PlayBGM();
       
    }

    void Finalize()
    {
        StopBGM();
        imgMapChip.reset();
        imgPlayer.reset();
        imgFireball.reset();
        imgMagicCircle.reset();
        imgEnemy.reset();
        imgNoteSheet.reset();
        font.reset();
        imgUIMagicBase.reset();
        imgUIMagicCircle.reset();
        imgUIMagicFireball.reset();
        imgUIMagicEmpty.reset();

        imgUISongList.reset();

        imgUIScoreBase.reset();


        bullets.clear();
        enemies.clear();
        notes.clear();
        noteBuf.clear();
        posBuf.clear();

        canFireball = false;
        circleReady = false;
        circleActive = false;

        circleTimer = 0.0f;
        enemySpawnTimer = 0.0f;
        enemyMoveTimer = 0.0f;
        
    }

    TaskFlag UpDate()
    {
        UpDateDeltaTime();              //  deltaTime
        BGM_KeepLoop();
        auto inp = ge->in1->GetState();

        Map_UpData(mapData);

        TaskFlag rtv = TaskFlag::Game;
        /*if (true == inp.ST.down) {
            rtv = TaskFlag::Title;
        }*/

        Angle moved = Player_UpDate(player);
        PushNote(moved);

        if (moved != Angle::Non) {
            SpawnStepNote(moved);

            posBuf.push_back(ML::Point{ player.x, player.y });
            if ((int)posBuf.size() > 9) posBuf.pop_front();

            int tlx, tly;
            if (CheckLoop3x3AndGetTopLeft(posBuf, tlx, tly)) {
                circleReady = true;
                circleX = tlx;
                circleY = tly;

                posBuf.clear();
                posBuf.push_back(ML::Point{ player.x, player.y });
            }
        }

        if (inp.B2.down) // X
        {
            if (circleReady && !circleActive) {
                circleActive = true;
                circleReady = false;
				circleTimer = CIRCLE_LIFE_FRAMES;   // seconds
            }
            else if (circleActive && IsPlayerInCircle3x3()) {
                SpawnFireball();
            }
            else if (canFireball) {
                SpawnFireball();
                noteBuf.clear();
                canFireball = false;
            }
        }

        if (circleActive) {
            circleTimer -= deltaTime;
            if (circleTimer <= 0.0f) {
                circleActive = false;
                circleTimer = 0.0f;
            }
        }

        Bullets_UpDate();
        Notes_UpDate();

        enemySpawnTimer -= deltaTime;
        if (enemySpawnTimer <= 0.0f) {
            enemySpawnTimer += ENEMY_SPAWN_INTERVAL;
            Enemies_Spawn2();
        }

        Enemies_UpDate();
        //============================================================
        // GameOver 
        //============================================================
        bool isGameOver = false;

        for (auto& e : enemies) {
            if (!e.alive) continue;

			// (1) hit player
            if (e.x == player.x && e.y == player.y) {
                isGameOver = true;
                break;
            }

			// (2) monster reach goal
            if (e.x <= ENEMY_GOAL_X) {
                isGameOver = true;
                break;
            }
        }

        if (isGameOver) {
            gResultScore = score;          // point
            return TaskFlag::Ending;       //  Ending
        }
        return rtv;
    }

    void Render()
    {
        Map_Render(mapData);
        Notes_Render();

        if (circleActive) {
            ML::Box2D dst(circleX * 32, circleY * 32, 96, 96);
            ML::Box2D src(0, 0, 96, 96);
            imgMagicCircle->Draw(dst, src);
        }

        Player_Render(player);
        Enemies_Render();
        Bullets_Render();
        UI_Render();
    }


    //============================================================
    // 10) Map / Player / Input helper
    //============================================================
    void Map_UpData(MapData& md_) { (void)md_; }

    void Map_Render(MapData& md_)
    {
        for (int y = 0; y < 12; ++y) {
            for (int x = 0; x < 20; ++x) {
                ML::Box2D draw(0, 0, 32, 32);
                draw.Offset(x * 32, y * 32);
                imgMapChip->Draw(draw, md_.chip[md_.arr[y][x]]);
            }
        }
    }

    void Map_Initialize(MapData& md_)
    {
        for (int c = 0; c < 64; ++c) {
            int x = (c % 8);
            int y = (c / 8);
            md_.chip[c] = ML::Box2D(x * 32, y * 32, 32, 32);
        }

        for (int y = 0; y < 12; ++y) {
            for (int x = 0; x < 20; ++x) {
                md_.arr[y][x] = 0;
            }
        }
    }

    bool Map_load(MapData& md_, int n_)
    {
        std::string filePath = "./data/map/Map" + std::to_string(n_) + ".txt";
        std::ifstream fin(filePath);
        if (!fin) { return false; }

        for (int y = 0; y < 12; ++y) {
            for (int x = 0; x < 20; ++x) {
                fin >> md_.arr[y][x];
            }
        }
        fin.close();
        return true;
    }

    static bool IsBlockedTile(int tileId)
    {
        switch (tileId)
        {
        case 4: case 5: case 6:
        case 8:
        case 9: case 10: case 11:
        case 12: case 13: case 14:
        case 16:
        case 20: case 21: case 22:
        case 24:
        case 28: case 29: case 30:
        case 32:
        case 40:
        case 48:
            return true;
        default:
            return false;
        }
    }

    bool Map_CheckHit(MapData& md_, ML::Point p_)
    {
        if (p_.x < 0) return true;
        if (p_.y > 11) return true;
        if (p_.x >= 20) return true;
        if (p_.y < 0) return true;

        int tileId = md_.arr[p_.y][p_.x];
        if (IsBlockedTile(tileId)) return true;

        return false;
    }

    void Map_ChangeClip(MapData& md_, ML::Point p_)
    {
        if (p_.x < 0) return;
        if (p_.y > 11) return;
        if (p_.x >= 20) return;
        if (p_.y < 0) return;

        md_.arr[p_.y][p_.x] = (md_.arr[p_.y][p_.x] += 1) % 4;
    }

    void Player_Initialize(Chara& c_, int x_, int y_, Angle a_)
    {
        c_.x = x_;
        c_.y = y_;
        c_.angle = a_;
    }

    Angle Player_UpDate(Chara& c_)
    {
        auto inp = ge->in1->GetState();
        ML::Point pre = { c_.x , c_.y };

        bool move = true;
        Angle movedDir = Angle::Non;

        switch (Angle4(inp)) {
        case Angle::Left:
            c_.x -= 1; c_.angle = Angle::Left; movedDir = Angle::Left; break;
        case Angle::Right:
            c_.x += 1; c_.angle = Angle::Right; movedDir = Angle::Right; break;
        case Angle::Up:
            c_.y -= 1; c_.angle = Angle::Up; movedDir = Angle::Up; break;
        case Angle::Down:
            c_.y += 1; c_.angle = Angle::Down; movedDir = Angle::Down; break;
        default:
            move = false; break;
        }

        if (move) {
            ML::Point now = { c_.x, c_.y };
            if (Map_CheckHit(mapData, now)) {
                c_.x = pre.x;
                c_.y = pre.y;
                return Angle::Non;
            }
            lastStepTile = pre;
            return movedDir;
        }
        return Angle::Non;
    }

    void Player_Render(Chara& c_)
    {
        ML::Box2D draw(0, -12, 32, 32);
        draw.Offset(c_.x * 32, c_.y * 32);
        ML::Box2D src = playerImage[(int)c_.angle];
        imgPlayer->Draw(draw, src);
    }

    Angle Angle4(XI::VGamePad& inp_)
    {
        int x = 1, y = 1;
        if (inp_.LStick.BL.down) { x -= 1; }
        if (inp_.LStick.BR.down) { x += 1; }
        if (inp_.LStick.BU.down) { y -= 1; }
        if (inp_.LStick.BD.down) { y += 1; }

        Angle mat[3][3] = {
            {Angle::Left, Angle::Up,   Angle::Up},
            {Angle::Left, Angle::Non,  Angle::Right},
            {Angle::Down, Angle::Down, Angle::Right},
        };
        return mat[y][x];
    }
    static void UI_Render()
    {
        // --- ui x3 ---
        ML::Box2D left(LEFT_X, HUD_Y, COL_W, HUD_H);      // 213x66
        ML::Box2D mid(MID_X, HUD_Y, COL_W, HUD_H);      // 213x66
        ML::Box2D right(RIGHT_X, HUD_Y, RIGHT_W, HUD_H);     // 214x66

        //====================================================
        // 1) 左：skill
        //====================================================
        imgUIMagicBase->Draw(left, ML::Box2D(0, 0, COL_W, HUD_H));

        // 48x48
        const int ICON_W = 48;
        const int ICON_H = 48;

        // left (need tyosei.....
        int iconX = left.x + (left.w - ICON_W) / 2 +59 ;
        int iconY = left.y + (left.h - ICON_H) / 2 - 2;

        ML::Box2D iconDst(iconX, iconY, ICON_W, ICON_H);

       

        bool canCircle = (circleReady || circleActive);
        bool canFire = ((circleActive && IsPlayerInCircle3x3()) || canFireball);

        if (canFire)
        {
            imgUIMagicFireball->Draw(iconDst, ML::Box2D(0, 0, ICON_W, ICON_H));
        }
        else if (canCircle)
        {
            imgUIMagicCircle->Draw(iconDst, ML::Box2D(0, 0, ICON_W, ICON_H));
        }
        else
        {
            imgUIMagicEmpty->Draw(iconDst, ML::Box2D(0, 0, ICON_W, ICON_H));
        }

        //====================================================
        // 2) m list
        //====================================================
        imgUISongList->Draw(mid, ML::Box2D(0, 0, COL_W, HUD_H));

        //====================================================
        // 3) ui score
        //====================================================
        imgUIScoreBase->Draw(right, ML::Box2D(0, 0, RIGHT_W, HUD_H));

        
        int textX = right.x + 120;
        int textY = right.y + 18; // 18/22/26 

        char buf[64];
        sprintf_s(buf, "%d", score);

       
        font->Draw(ML::Box2D(textX, textY, 200, 32), buf);
    }


}
