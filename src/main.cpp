#include <iostream>
#include <string>

// 加速減速で1サイクル消費


struct CarInfo {
    int velocity {0};
    int fuel {0};
    int position_x {0};
    int position_y {0};
    int orientation {0}; // North: 0, East: 1, South: 2, West: 3;
};


std::string InputCommand() {
    std::string Input;

    std::cout << "コマンドを入力して下さい. " << std::endl;
    while (true) {
        std::cout << "[ turn left | turn right | continue straight | " << std::endl;
        std::cout << "  accelerate | decelerate | stop ]" << std::endl;
        std::cout << "==> ";
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


/* 地図内かつ道路上にいるか判定
bool DetermineArea (std::string Input, CarInfo MyCar) {
    bool flag = false;
    int NextOrientation; // コマンド入力を加味した次に向く方位

    // 方位角チェック, [turn left]は現在の方位から反時計回りに変換, [turn right]は現在の方位から時計回りに変換
    if (Input == "turn right") {
        NextOrientation = (MyCar.orientation + 1) % 4;
    } else if (Input == "turn left") {
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
    
    // 自車の方位に応じてエリア外判定をする
    // 北
    /*int move {1};
    if ( NextOrientation == 0 ) {
        while ()
    }*/
//} */


int main () {
    CarInfo MyCar;
    std::string Input;
    bool AreaFlag {false};

    while (true) {
        while (true) {
            // コマンドを格納
            Input = InputCommand();
            
            bool AreaFlag {true};
            if ((Input == "turn left") ||  (Input == "turn right") || (Input == "continue straight")) {
                // 地図内、道路内判定
                //AreaFlag = DetermineArea(Input, MyCar);
            }

            if (AreaFlag = true) {
                break;
            } else {
                std::cout << " [注意] エリア外となります. 方向を変えるか、減速して下さい. " << std::endl;
            }
        }
    }

    return 0;
}