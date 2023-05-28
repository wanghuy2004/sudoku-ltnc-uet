# test-ltnc

cách chạy trò chơi

ở phần new terminal trong vscode, chạy lệnh minGW32-make để build file exe.
sau đó chạy lệnh ./game để run file exe đó

hoặc ra thư mục chứa code để chạy file game.exe

font chữ dùng ở đây là DIGITAL SANS EF

--------------------------------------BÁO CÁO--------------------------------------
tạo được đồng hồ đếm thời gian khi người chơi chơi game

		// Tính thời gian từ thời điểm bắt đầu đếm của trò chơi
		time_t difference = time(NULL) - startTimer;

		// Định dạng thời gian để hiển thị lên màn hình
		tm* formattedTime = gmtime(&difference); //localtime_s(&formattedTime, &difference); của ver1.cpp
		char timer[80];
		strftime(timer, sizeof(timer), "%H:%M:%S", formattedTime);

		// Hiển thị thời gian trên màn hình
		SDL_Texture* timerTexture = nullptr;
		SDL_Color fontColour = { 0, 0, 0, SDL_ALPHA_OPAQUE }; // black
		loadTexture(timerTexture, timer, fontColour);
		mTimer.setTexture(timerTexture);
		mTimer.renderButton(mRenderer);
		mTimer.centerTextureRect();
		mTimer.renderTexture(mRenderer);
		SDL_DestroyTexture(timerTexture);
		timerTexture = nullptr;
    

UPDATE:
sửa lỗi đồng hồ bị lỗi khi bắt đầu game thì đếm từ 07:00:00 ở ver1

