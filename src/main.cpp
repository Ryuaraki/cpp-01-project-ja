#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>


struct CarInfo {
    int velocity {10};
    int fuel {10}; 
    int position_x {0};
    int position_y {0};
    int orientation {0}; // North: 0, East: 1, South: 2, West: 3;
};

// inputをdelimiterで分割
std::vector<std::string> splitLine(const std::string& input, char delimiter) {
    std::istringstream stream(input);
    std::string field;
    std::vector<std::string> result;

    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }
    
    return result;
}


void loadMap (std::vector<std::vector<std::string>>& output_map) {
    std::string path = "../citymap.csv"; // 読み取り元
    std::ifstream ifs(path);

    if (!ifs.is_open()) {
        throw std::runtime_error("Cannot open file"); 
    } else {
        std::string line;

        while (getline(ifs, line)) {
            std::vector<std::string> strvec = splitLine(line, ',');
            output_map.push_back(strvec);
        }
    }
}


void showMap (const std::vector<std::vector<std::string>>& input_map) {

    // 地図をコンソール画面に表示する
    std::cout << "---------------------------[City Map]------------------------------------" << std::endl;
    for (int i=0; i < input_map.size(); ++i) {
        for (int j=0; j < input_map[i].size(); ++j) {
            std::cout << input_map[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "---------------------------[City Map]------------------------------------" << std::endl;
}

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
        if( (position_x >=0) && (position_x < map_data.size()) && (position_y >= 0) && (position_y < map_data[0].size()) ){
            std::cout << "初期方位(北[0], 東[1], 南[2], 西[3]): ";
            std::cin >> orientation;
            if ( orientation >= 0 && orientation <= 3 ) {
                break; // 初期位置と初期方位が正しく入力されたので、入力終了.
            } else {
                std::cout << "初期方位入力エラー: 初期方位は0~3の数字を入力して下さい." << std::endl;
            }
        } else {
            std::cout << "初期位置入力エラー: が地図の範囲外です. " << std::endl;
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
    switch (orientation) {
        case 0: { // 北向き
            map_for_visualize[position_x][position_y] = "↑";
            break;
        }
        case 1: { // 東向き
            map_for_visualize[position_x][position_y] = "→";
            break;
        }
        case 2: { // 南向き
            map_for_visualize[position_x][position_y] = "↓";
            break;
        }
        default: { // 西向き
            map_for_visualize[position_x][position_y] = "←";
            break;
        }
    }

    // 自車位置込みのマップ表示
    showMap(map_for_visualize);
    
    return;
}

void showCarInfo (const CarInfo& MyCar) {
    std::cout << "[CarInfomation]" << std::endl;
    std::cout << "Position(" << MyCar.position_x << ", " << MyCar.position_y << "), ";
    std::cout << "Velocity: " << MyCar.velocity << ", Fuel: " << MyCar.fuel << std::endl;
    std::cout << "------" << std::endl;
}

std::string InputCommand() {
    std::string Input;

    std::cout << "コマンドを入力して下さい. " << std::endl;
    while (true) {
        std::cout << "[ turn left | turn right | continue straight | " << std::endl;
        std::cout << "  accelerate | decelerate | stop ]" << std::endl;
        std::cout << "==> ";
        std::cin.ignore(); // 最後の入力した改行コードを無視
        std::getline(std::cin, Input); // スペース区切りの入力を一つの文字列として扱う

        if((Input == "turn left") ||  (Input == "turn right") || (Input == "continue straight")
            || (Input == "accelerate") || (Input == "decelerate") || (Input == "stop")) {
                break;
        } else {
            std::cout << "入力信号が認識できませんでした. もう一度入力して下さい." << std::endl;
        }
    }

    return Input;
}

// 自車状態判定
//  - 自車が停止状態の時で移動する時は


// 地図内かつ道路上にいるか判定
bool DetermineArea (std::string command, const CarInfo& MyCar, const std::vector<std::vector<std::string>>& map_data) {
    int NextOrientation {0}; // コマンド入力を加味した次に向く方位

    // 方位角チェック, [turn left]は現在の方位から反時計回りに変換, [turn right]は現在の方位から時計回りに変換
    if (command == "turn right") {
        NextOrientation = (MyCar.orientation + 1) % 4;
    } else if (command == "turn left") {
        // 北(0)から西(3)への方位変換
        if ( (MyCar.orientation - 1) < 0 ) {
            NextOrientation = 3; // 西の方位(3)を入力
        } else { 
            NextOrientation = MyCar.orientation -1;
        }
    } else {
        // 現状の方位のまま
        NextOrientation = MyCar.orientation;
    }

    // NextOrientation = MyCar.orientation()
    
    // 自車の方位に応じてエリア外判定をする. TODO: 道路上判定も実施
    // 自車から進行方向に対して通る全てのセルを判定.
    int move {0};
    bool AreaFlag {true};

    while ( move < MyCar.velocity ) {
        if ( NextOrientation == 0 ) {       // 方角が北(0)の場合
            if ( (MyCar.position_y - move) < 0 ) 
                AreaFlag = false; // エリア外
        } else if ( NextOrientation == 1) { // 方角が東(1)の場合
            if ( (MyCar.position_x + move) > (map_data[0].size() - 1) ) 
                AreaFlag = false; // エリア外
        } else if (NextOrientation == 2) {  // 方角が南(2)の場合
            if ( (MyCar.position_y + move) > (map_data.size() - 1) ) 
                AreaFlag = false; // エリア外
        } else { // 方角が西(3)の場合
            if ( (MyCar.position_x - move) < 0 )
                AreaFlag = false; // エリア外 
        }
        move++;
std::cout << "Debug" << std::endl;
    }
    return AreaFlag;
}


int main () {
    CarInfo MyCar;
    std::string command;
    std::vector<std::vector<std::string>> map_data; // 街の地図情報の格納用
    std::vector<std::vector<std::string>> map_for_visualize; // 描画用の地図情報格納(自車位置込み)

    bool AreaFlag {false};

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

    while (true) {
        while (true) {
            // コマンドを格納
            command = InputCommand();
            bool AreaFlag {true};
            // 移動を行うコマンドは地図内, 道路内判定を実施
            if ((command == "turn left") ||  (command == "turn right") || (command == "continue straight")) {
                // 地図内、道路内判定
                AreaFlag = DetermineArea(command, MyCar, map_data);
            }
            
            // エリア外の場合はもう一度コマンド入力を求める
            if (AreaFlag = true) {
                break;
            } else {
                std::cout << " [注意] エリア外となります. 方向を変えるか、減速して下さい. " << std::endl;
            }
        }
        // 自車の状態を更新する
        std::cout << "これ以降の処理は間にいませんでした。。" << std::endl; 
        break;
    }

    return 0;
}