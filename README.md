# cpp-01-project-ja

## 2D Car Game
用意した地図内で、車を操作してゴール(G)を目指すゲーム    

### 1. まずはじめに
  cpp-01-project-jaフォルダ内に、citymap.csvがあるので、そのファイルを開いて地図を作成する.  
  
    [○]: 走行禁止エリア（英語の小文字のo)  
    [×]: 走行禁止エリア  
    [F]: ガソリンスタンド（燃料(Fuel)を補給）  
    [T]: LIFE TIMEチャージスポット  
    [G]: GOAL地点  

### 2. 実行手順
    cpp-01-project-ja/srcフォルダに移動して、「g++ -std=c++17 -o main main.cpp」コマンドを実行しビルドする.      
    「./main」 を実行してゲーム開始  

### 3. ゲームルール
 <ゲームクリア条件>  
    [G]マスに止まる事. ただし、燃料(Fuel)やLIFE TIMERが0になるとゲームオーバーとなる.  
  
 <注意点>  
    一度通った場所は走行禁止エリアに変わる.
  
 <自車>  
    自車は地図上に矢印で表示される. 矢印の向きは進む方角を表す.  
   
 <コマンド一覧>  
    [l]:turn left, [r]:turn right, [c]continue straight, [a]:accelerate, [d]:decelerate, [s]:stop