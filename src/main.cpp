#include <iostream>
#include <iomanip> // for setw()
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>


struct CarInfo {
    int velocity {1};
    int fuel {30}; 
    int position_x {0};
    int position_y {0};
    int orientation {0}; // North: 0, East: 1, South: 2, West: 3;
    bool goal_flag {false};
};

// timer
int life_timer = 30;

// 定数
const float FUEL_COEFFICIENT = 1.25; // 燃料係数
const int MAX_FUEL = 30; // 最大燃料
const int PLUS_TIMER = 15; // 時間加算

// 使用関数
void loadMap (std::vector<std::vector<std::string>>& output_map);
std::vector<std::string> splitLine(const std::string& input, char delimiter);
void showMap (const std::vector<std::vector<std::string>>& input_map);
void setInitialPosition (const std::vector<std::vector<std::string>>& map_data, CarInfo& MyCar);
void showCarInfo (const CarInfo& MyCar);
char InputCommand ();
bool DetermineArea (char command, const CarInfo& MyCar, std::vector<std::vector<std::string>>& map_data);
int setNextOrientation (char command, int orientation);
void updateCarInfo (char command, CarInfo& MyCar, const std::vector<std::vector<std::string>>& map_data);


// 自車状態判定
//  - 自車が停止状態の時で移動する時は



/////////////////////////////
//  main関数
/////////////////////////////
int main () {
    CarInfo MyCar;
    char command; // ユーザーの入力コマンド格納用
    std::vector<std::vector<std::string>> map_data; // 街の地図情報の格納用
    std::vector<std::vector<std::string>> map_for_visualize; // 描画用の地図情報格納(自車位置込み)
    //bool AreaFlag {false};

    // 街の地図を読み込み、2次元配列に格納
    try {
        loadMap(map_data);
        std::cout << "街の地図が正常に読み込まれました." << std::endl;
        showMap(map_data);
    } catch ( const std::runtime_error& e ) {
        std::cerr << " Error: " << e.what() << std::endl;
        return 1;
    }

    // 自車の初期位置と初期方位を入力する
    setInitialPosition(map_data, MyCar);
    // 自車情報表示
    showCarInfo(MyCar);
    std::cout << "::::::LIFE TIMER:::::::" << life_timer << "[s]:::::::" << std::endl;

    while (true) {
        while (true) {
            // コマンドを格納
            command = InputCommand();
            
            bool AreaFlag {true};

            // 移動を行うコマンドは地図内, 道路内判定を実施
            if ((command == 'l') ||  (command == 'r') || (command == 'c')) {
                if (MyCar.velocity == 0) {
                    std::cout << " ++++ 速度が0です. 加速して下さい. ++++" << std::endl;
                    continue;
                }
                // 地図内、道路内判定
                AreaFlag = DetermineArea(command, MyCar, map_data);
            }
            
            // エリア内であればコマンド入力ループを抜けて, エリア外はもう一度コマンド入力を求める.
            if (AreaFlag == true) {
                break;
            }
        }
        // 自車の状態を更新する
        updateCarInfo(command, MyCar, map_data);

        // 自車情報表示
        showCarInfo(MyCar);
        
        // timer情報
        life_timer--;
        std::cout << "::::::LIFE TIMER::::::: " << life_timer << "[s]:::::::" << std::endl;

        // 終了判定 
        if (MyCar.goal_flag == true) {
            std::cout << "~~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "~~~~ Goalに到着 ~~~~~" << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~" << std::endl;
            break;
        } else if (MyCar.fuel <= 0) {
            std::cout << "###############################################" << std::endl;
            std::cout << "#############  Fuel is empty  #################" << std::endl;
            std::cout << "###############################################" << std::endl;
            break;
        } else if (life_timer <= 0) {
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!  TIME UP  !!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            break;
        } else if ( (MyCar.position_x > 0) && (MyCar.position_x < map_data[0].size() - 1) 
                && (MyCar.position_y > 0) && (MyCar.position_y < map_data.size() - 1)) { // 移動するスペースがない場合も終了
                // 四方向[×]に囲まれている.
                if ( (map_data[MyCar.position_y - 1][MyCar.position_x] == "×") && (map_data[MyCar.position_y][MyCar.position_x + 1] == "×")
                    && (map_data[MyCar.position_y + 1][MyCar.position_x] == "×") && (map_data[MyCar.position_y][MyCar.position_x - 1] == "×") ) {
                    std::cout << "**********************************" << std::endl;
                    std::cout << "*** 走行するスペースがありません ***" << std::endl;
                    std::cout << "***********************************" << std::endl;
                    break;
                }    
        } else {
            // 何もしない.
        }
    }

    std::cout << "ゲーム終了" << std::endl;
    return 0;
}

/////////////////////////////
// 地図の読み込み
/////////////////////////////
void loadMap (std::vector<std::vector<std::string>>& output_map) {
    std::string path = "../citymap.csv"; // 読み取り元
    std::ifstream ifs(path);

    if (!ifs.is_open()) {
        throw std::runtime_error("Cannot open file"); 
    } else {
        std::string line;

        while (std::getline(ifs, line)) {
            std::vector<std::string> strvec = splitLine(line, ',');
            output_map.push_back(strvec);
        }
    }
}

/////////////////////////////
// inputをdelimiterで分割
/////////////////////////////
std::vector<std::string> splitLine(const std::string& input, char delimiter) {
    std::istringstream stream(input);
    std::string field;
    std::vector<std::string> result;

    while (std::getline(stream, field, delimiter)) {
        result.push_back(field);
    }
    
    return result;
}

/////////////////////////////
// 地図の表示
/////////////////////////////
void showMap (const std::vector<std::vector<std::string>>& input_map) {

    // 地図をコンソール画面に表示する
    std::cout << "---------------------------[City Map]------------------------------------" << std::endl;
    // 行番号
    std::cout << " ";
    for (int i=0; i < input_map.size(); ++i) {
        std::cout << " " << std::setw(2) << i; 
    }
    std::cout << std::endl;
    for (int i=0; i < input_map.size(); ++i) {
        // 列番号
        std::cout << std::setw(2) << i << " ";
        for (int j=0; j < input_map[i].size(); ++j) {
            std::cout << input_map[i][j] << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << "---------------------------[City Map]------------------------------------" << std::endl;
}

/////////////////////////////
// 初期位置設定
/////////////////////////////
void setInitialPosition (const std::vector<std::vector<std::string>>& map_data, CarInfo& MyCar) {
    int position_x {0};
    int position_y {0};
    int orientation {0};
    
    std::cout << "自車の初期位置と初期方位を入力して下さい." << std::endl;
    while (true) {
        std::cout << "初期位置x: ";
        std::cin >> position_x;
        std::cout << "初期位置y: ";
        std::cin >> position_y;

        // 初期位置がMap内か
        if( (position_y >=0) && (position_y < map_data.size()) && (position_x >= 0) && (position_x < map_data[0].size()) ) {
            // 走行可能領域か
            if (map_data[position_y][position_x] != "×") {
                std::cout << "初期方位(北[0], 東[1], 南[2], 西[3]): ";
                std::cin >> orientation;
                if ( orientation >= 0 && orientation <= 3 ) {
                    break; // 初期位置と初期方位が正しく入力されたので、入力終了.
                } else {
                    std::cout << "初期方位入力エラー: 初期方位は0~3の数字を入力して下さい." << std::endl;
                }
            } else {
                std::cout << "初期位置入力エラー: 走行不可領域です. そこには車両を置けません." << std::endl;
            }
        } else {
            std::cout << "初期位置入力エラー: 地図の範囲外です. " << std::endl;
        }
        std::cout << "もう一度初めから入力して下さい. " << std::endl;
    } 

    // 初期位置格納
    MyCar.position_x = position_x;
    MyCar.position_y = position_y;
    MyCar.orientation = orientation;
    
    std::vector<std::vector<std::string>> map_for_visualize; // 描画用の地図情報格納(自車位置込み)
    map_for_visualize = map_data;
    // 描画用に自車位置格納
    switch (MyCar.orientation) {
        case 0: { // 北向き
            map_for_visualize[MyCar.position_y][MyCar.position_x] = "↑";
            break;
        }
        case 1: { // 東向き
            map_for_visualize[MyCar.position_y][MyCar.position_x] = "→";
            break;
        }
        case 2: { // 南向き
            map_for_visualize[MyCar.position_y][MyCar.position_x] = "↓";
            break;
        }
        default: { // 西向き
            map_for_visualize[MyCar.position_y][MyCar.position_x] = "←";
            break;
        }
    }

    // 自車位置込みのマップ表示
    showMap(map_for_visualize);
    
    return;
}

/////////////////////////////
// 車の情報表示
/////////////////////////////
void showCarInfo (const CarInfo& MyCar) {
    std::cout << "[CarInfomation]" << std::endl;
    std::cout << "Position(" << MyCar.position_x << ", " << MyCar.position_y << "), ";
    switch (MyCar.orientation) {
        case 0: { // 北向き
            std::cout << "Orientation(北[0])" << std::endl;
            break;
        }
        case 1: { // 東向き
            std::cout << "Orientation(東[1])" << std::endl;
            break;
        }
        case 2: { // 南向き
            std::cout << "Orientation(南[2])" << std::endl;
            break;
        }
        default: { // 西向き
            std::cout << "Orientation(西[3])" << std::endl;
            break;
        }
    }
    std::cout << "Velocity: " << MyCar.velocity << ", Fuel: " << MyCar.fuel << std::endl;
    std::cout << "------" << std::endl;
}

/////////////////////////////
// コマンド入力
/////////////////////////////
char InputCommand() {
    //std::string Input;
    char Input;

    std::cout << "コマンドを入力して下さい. " << std::endl;
    while (true) {
        std::cout << "[ [l]:turn left | [r]:turn right | [c]continue straight | " << std::endl;
        std::cout << "  [a]:accelerate | [d]:decelerate | [s]:stop ]" << std::endl;
        std::cout << "==> ";
        std::cin >> Input;
        // std::cin.ignore(); // 最後の入力した改行コードを無視
        // std::getline(std::cin, Input); // スペース区切りの入力を一つの文字列として扱う, 入力を文字列から1文字に変更したので不要

        if((Input == 'l') ||  (Input == 'r') || (Input == 'c')
            || (Input == 'a') || (Input == 'd') || (Input == 's')) {
                break;
        } else {
            std::cout << "入力信号が認識できませんでした. もう一度入力して下さい." << std::endl;
        }
    }

    return Input;
}

/////////////////////////////
// 地図内かつ道路上にいるか判定
/////////////////////////////
bool DetermineArea (char command, const CarInfo& MyCar, std::vector<std::vector<std::string>>& map_data) {
    
    // 方位角変換
    int next_orientation {0}; // 北: 0
    next_orientation = setNextOrientation(command, MyCar.orientation);
    
    // 自車の方位に応じてエリア外判定をする.
    // 自車から進行方向に対して通る全てのセルを判定.
    int move {1};
    bool AreaFlag {true};

    // 通過経路を記録する用のマップ
    std::vector<std::vector<std::string>> update_map;
    update_map = map_data;

    // 自車に近いセルから通れるか判定
    while ( move <= MyCar.velocity ) {
        if ( next_orientation == 0 ) {       // 方角が北(0)の場合
            if ( (MyCar.position_y - move) < 0 ) {
                AreaFlag = false;
                std::cout << " <<注意>> エリア外となります. 方向を変えるか、減速して下さい. " << std::endl;
                break;
            } else if (map_data[MyCar.position_y - move][MyCar.position_x] == "×") {
                AreaFlag = false;
                std::cout << " <<注意>> 走行禁止領域となります. 方向を変えるか、減速して下さい. " << std::endl;
                break;
            } else {
                if (move != MyCar.velocity) {
                    // 最終到達点以外の通過場所を通行禁止[×]にする
                    update_map[MyCar.position_y - move][MyCar.position_x] = "x";
                }
            }
        } else if (next_orientation == 1) { // 方角が東(1)の場合
            if ( (MyCar.position_x + move) > (map_data[0].size() - 1) ) {
                AreaFlag = false;
                std::cout << " <<注意>> エリア外となります. 方向を変えるか、減速して下さい. " << std::endl;
                break;
            } else if (map_data[MyCar.position_y][MyCar.position_x + move] == "×") {
                AreaFlag = false;
                std::cout << " <<注意>> 走行禁止領域となります. 方向を変えるか、減速して下さい. " << std::endl;
                break;
            } else {
                if (move != MyCar.velocity) {
                    // 最終到達点以外の通過場所を通行禁止[×]にする
                    update_map[MyCar.position_y][MyCar.position_x + move] = "×";
                }
            }
        } else if (next_orientation == 2) {  // 方角が南(2)の場合
            if ( (MyCar.position_y + move) > (map_data.size() - 1) ) {
                AreaFlag = false;
                std::cout << " <<注意>> エリア外となります. 方向を変えるか、減速して下さい. " << std::endl;
                break;
            } else if (map_data[MyCar.position_y + move][MyCar.position_x] == "×") {
                AreaFlag = false;
                std::cout << " <<注意>> 走行禁止領域となります. 方向を変えるか、減速して下さい. " << std::endl;
                break;
            } else {
                if (move != MyCar.velocity) {
                    // 最終到達点以外の通過場所を通行禁止[×]にする
                    update_map[MyCar.position_y + move][MyCar.position_x] = "×";
                }
            }
        } else { // 方角が西(3)の場合
            if ( (MyCar.position_x - move) < 0 ) {
                AreaFlag = false;
                std::cout << " <<注意>> エリア外となります. 方向を変えるか、減速して下さい. " << std::endl;
                break;
            } else if (map_data[MyCar.position_y][MyCar.position_x - move] == "×") {
                AreaFlag = false;
                std::cout << " <<注意>> 走行禁止領域となります. 方向を変えるか、減速して下さい. " << std::endl;
                break;
            } else {
                if (move != MyCar.velocity) {
                    // 最終到達点以外の通過場所を通行禁止[×]にする
                    update_map[MyCar.position_y][MyCar.position_x - move] = "×";
                }
            }        
        }
     
        // 次に向かうセルへ移動
        move++;
    }
    
    // 地図の情報更新
    if (AreaFlag == true) {
        if (MyCar.velocity != 0) {
            // 通過経路を元のmapに反映
            map_data = update_map;
            // 移動前の位置も通行禁止とする.
            map_data[MyCar.position_y][MyCar.position_x] = "×";
        }
    } else {
        // 何もしない.
    }

    return AreaFlag;
}


/////////////////////////////
// 方位の変換
/////////////////////////////
int setNextOrientation (char command, int current_orientation) {

    int next_orientation {0}; // コマンド入力を加味した次に向く方位
    
    // 方位角変換. [turn left]は現在の方位から反時計回りに変換, [turn right]は現在の方位から時計回りに変換
    if (command == 'r') {
        next_orientation = (current_orientation + 1) % 4;
    } else if (command == 'l') {
        // 北(0)から西(3)への方位変換
        if ( (current_orientation - 1) < 0 ) {
            next_orientation = 3; // 西の方位(3)を入力
        } else { 
            next_orientation = current_orientation -1;
        }
    } else {
        // 現状の方位のまま
        next_orientation = current_orientation;
    }
    
    return next_orientation;
}

/////////////////////////////
// 自車の状態を更新
/////////////////////////////
void updateCarInfo (char command, CarInfo& MyCar, const std::vector<std::vector<std::string>>& map_data) {

    // 移動を伴わないコマンド(加速, 減速, 停止)の場合は速度のみを更新
    if (command == 'a') {   // 加速
        if (MyCar.velocity == 0) { // 速度0の場合は+1する
            MyCar.velocity = 1;
        } else {
            MyCar.velocity *= 2; 
        }
        // 燃料更新(加速も燃料消費)
        MyCar.fuel -= 1;
    } else if (command == 'd') { // 減速
        MyCar.velocity /= 2;
    } else if (command == 's') { // 停止
        MyCar.velocity = 0;
    } else { // 移動を伴うコマンド(左折, 右折, 直進)は位置を更新
        // 方位変換
        MyCar.orientation = setNextOrientation(command, MyCar.orientation);
        if (MyCar.orientation == 0) { // 北(0)向き
            MyCar.position_y -= MyCar.velocity;
        } else if (MyCar.orientation == 1) { // 東(1)向き
            MyCar.position_x += MyCar.velocity;
        } else if (MyCar.orientation == 2) { // 南(2)向き
            MyCar.position_y += MyCar.velocity;
        } else { // 西(3)向き
            MyCar.position_x -= MyCar.velocity;
        }
        // 燃料更新(現在速度に燃料係数をかける)
        int fuel;
        fuel = MyCar.fuel - static_cast<int>(MyCar.velocity * FUEL_COEFFICIENT);
        if (fuel < 0) {
            MyCar.fuel = 0;
        } else {
            MyCar.fuel = fuel;
        }
    }

    std::vector<std::vector<std::string>> map_for_visualize; // 描画用の地図情報格納(自車位置込み)
    map_for_visualize = map_data;
    // 描画用に自車位置格納
    switch (MyCar.orientation) {
        case 0: { // 北向き
            map_for_visualize[MyCar.position_y][MyCar.position_x] = "↑";
            break;
        }
        case 1: { // 東向き
            map_for_visualize[MyCar.position_y][MyCar.position_x] = "→";
            break;
        }
        case 2: { // 南向き
            map_for_visualize[MyCar.position_y][MyCar.position_x] = "↓";
            break;
        }
        default: { // 西向き
            map_for_visualize[MyCar.position_y][MyCar.position_x] = "←";
            break;
        }
    }
    // 自車位置込みのマップ表示
    showMap(map_for_visualize);

    // イベントチェック
    if (map_data[MyCar.position_y][MyCar.position_x] == "F") {
        MyCar.fuel = MAX_FUEL;
        std::cout << "========\n  [F] ガソリンスタンドに到着!! 燃料補給!!  \n========" << std::endl;
    } else if (map_data[MyCar.position_y][MyCar.position_x] == "T") {
        life_timer += PLUS_TIMER;
        std::cout << "========\n  [T] LIFE TIMER CHARGING!!!!  \n========" << std::endl;
    } else if (map_data[MyCar.position_y][MyCar.position_x] == "G") {
        MyCar.goal_flag = true;
    } else {
        // 何もなし.
    }
}