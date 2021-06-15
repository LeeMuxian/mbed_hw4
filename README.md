# mbed_hw4

## Part1.  
這個part是要能夠倒車入庫，首先我的car.py(這是Xbee用來傳RPC的python檔)會傳RPCFunction call指令以及參數給我的mbed。如下圖。  
![](https://i.imgur.com/PotAuGX.png)  
第一個參數是d1，第二個參數是d2(這兩個參數都是車身與停車格的距離)，第三個參數是一個字串(west或east)代表的是該往右後方還是左後方倒車。  
成功傳給mbed之後，mbed就會開始倒車。我的作法是先往後倒車一小段距離，然後轉彎90度，最後再往後倒車一小段距離以致車子能完全停進停車格。  
在我的main.cpp裡面要倒車的距離的算法是一職try and error試出來的，算法跟d1以及d2有關以下為適用於我的車子的倒車距離的算法。  
![](https://i.imgur.com/rNBfk7y.png)  
![](https://i.imgur.com/apDZLyU.png)  
上面那段程式碼是轉90度前應該倒退的距離，而下面那段則是轉90度之後該倒退的距離。  
以下是我demo的雲端連結以及截圖。  
https://drive.google.com/file/d/1sphbMXXcn8It8yNTRm--VPVl0bLak3aI/view?usp=sharing  
![](https://i.imgur.com/eL10CkC.png)  
  
## Part2.  
這個Part是要我們用OpenMV去掃直線以讓車子盡量沿著直線走，如果歪掉能夠校正回來。我的OpenMV的會一直讀取畫面中的直線，但因為整個環境太多直線，所以我把白紙鋪在地上並且讓中間有一條直線可以明顯讀取，另外我讓
畫面呈現灰階而不是RGB的狀態使OpenMV更容易讀取到正確的直線。  
![](https://i.imgur.com/EB6L9yf.png)  
我的OpenMV會將RPC call以及讀到的直線的參數(線段兩端點的座標)利用Uart傳給我的mbed(不斷的傳)，讓我的mbed執行RPC function。  
![](https://i.imgur.com/VoPUMCE.png)  
我的main.cpp裡的RPC會把讀到的座標拿來做為需不需要校正的依據，如果偏右的話就往左校正，偏左就往右校正(校正力道是try and error試出來的)，如果在正負10(正中間為80，因此可接受範圍是70~90)的可接受的誤差範圍內的話就直走。我這裡一次RPC只會走100毫秒，
是為了讓車子能夠及時校正。因此會一直不斷重複執行這個RPC來保證校正的及時性。  
以下是我的demo的雲段連結以及截圖。  
https://drive.google.com/file/d/136fw5IkO-zduwfwM63S1B9jFcwosdUG5/view?usp=sharing  
![](https://i.imgur.com/Tj8gRJU.png)  

## Part3.  
這部分是要我們利用OpenMV讀取AprilTag並將車子開到AprilTag的正前方以垂直AprilTag，停下之後用ping測量距離。  
我在我OpenMV的code裡面也是將讀取到的AprilTag的參數(xyz三個座標的距離以及角度)包在RPC call字串裡面並用uart傳給mbed來呼叫mbed的RPC，我mbed裡的RPC function名字是calib。  
![](https://i.imgur.com/GVS6KiS.png)  
並且為了不讓RPC call的字串傳得過多次，我每十秒才會偵測AprilTag一次，並盡量一次RPC call就跑到位(就算沒有一次到位，10秒過後還會再偵測一次)。  
而在我的main.cpp裡則是收到RPC call之後會開是執行RPC function，在這個function裡我會把讀到的角度(水平角度即可)以及距離(AprilTag在畫面中橫向的距離即可)拿來做判斷，其餘角度以及距離則沒有使用到。  
首先，判斷車子是在AprilTag的左邊還是右邊(利用橫向的角度)來判斷要往哪一個方向走，以下我們以車子在AprilTag左邊來講解，另一邊同理。  
車子先轉到一個角度讓AprilTag在OpenMV畫面的正中間，轉到正中間之後接著就是算說還要轉多少度才會和AprilTag平行(用水平角度可以知道)。車子成功轉到和AprilTag平行之後就是直接直走，這個直走的距離在轉彎之前就
已經計算完畢，利用水平角度以及距離算tan就可以知道。走到該走的位置之後就是，直接轉90度就可以垂直面向AprilTag了。最後就是用超音波來測距離並用尺來確認。  
以下是我demo的雲端連結以及截圖。  
https://drive.google.com/file/d/1N7yN2hQoGcSBOwHaDhjuOsqiJk3NwbIQ/view?usp=sharing  
![](https://i.imgur.com/6vPE13w.png)  
![](https://i.imgur.com/byZSH1G.png)  
![](https://i.imgur.com/tLpvkfT.png)  
![](https://i.imgur.com/AVkHDq2.png)  
從demo影片中可以看的出來超波測的距離大概是37公分，而我放兩把尺在旁邊之後量測發現距離真的差不多是37公分。
