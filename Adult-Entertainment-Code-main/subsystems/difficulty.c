
int difficultyIncrease = 0;
void difficultyUpdate(int &difficulty,long &lastSwitchFlipTime, bool wasFlipped){
	if(wasFlipped){

		wasFlipped = false; // reset flipped variable

		if (time1[T1] - lastSwitchFlipTime < 1000){
			difficultyIncrease++; // adds a counter for how many times the user can flip it quickly
		}
		if (difficultyIncrease > 3 && difficulty != 4){
			difficulty++; // if counter passes a threshold the difficulty will increase by 1
		}

		lastSwitchFlipTime = time1[T1];
	}

	else if (time1[T1] - lastSwitchFlipTime > 5000 && difficulty != 0){
		difficulty --; // difficulty decrements if user is too slow
	}
}
