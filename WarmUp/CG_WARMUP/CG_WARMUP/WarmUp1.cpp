#include<iostream>
#include<random>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> uid(1, 9);

/*
1. 행렬 다루기
	4 x 4 행렬의 연산하는 프로그램을 구현한다.

	연산을 할 두 개의 행렬에는 한 자리 숫자 (1에서 9 사이의 숫자)를 랜덤하게 설정한다. 
	(사용자 입력X, 자동 배정)

	다음의 명령어로 수행한다. 종료 명령어를 입력할 때까지 명령을 연속적으로 수행할 수 있도록 한다.
	m: 행렬의 곱셈
	a: 행렬의 덧셈
	d: 행렬의 뺄셈
	r: 행렬식의 값 (Determinant)  입력한 2개의 행렬의 행렬식 값을 모두 출력한다.
	t: 전치 행렬(Transposed matrix)과 그 행렬식의 값, 설정된 두 개의 행렬에 모두 적용한다.
	e: 각 행에서 최소값을 찾아 그 값을 해당 행의 값에서 뺀다. 다시 누르면 원래대로 출력한다.
	f: 각 열에서 최대값을 찾아 그 값을 해당 열의 값에 더한다. 다시 누르면 원래대로 출력한다.
	+/-: 행렬의 모든 값에 +1/-1을 진행하고 값의 범위는 10으로 모듈라 연산하여 0~9 범위로 한다. 
		(즉, 값은 8 <-> 9 <- >0 <- >1 ... )
	s: 행렬의 값을 새로 랜덤하게 설정한다.
	q: 프로그램 종료

*/

void set(int matrix[4][4], int matrix2[4][4], int matrix3[4][4], int matrix4[4][4]) 
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix[i][j] = uid(gen);
			matrix2[i][j] = uid(gen);
			matrix3[i][j] = 0;
			matrix4[i][j] = 0;
		}
	}
}

void show(int matrix[4][4], int matrix2[4][4], int matrix3[4][4], int matrix4[4][4], int lastCommand)
{
	if (lastCommand == -1) {
		cout << "행렬 1 :       행렬 2 :     " << '\n';
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				cout << matrix[i][j] << " ";
				
			}
			cout << "       ";
			for (int j = 0; j < 4; j++) {
				cout << matrix2[i][j] << " ";
			}
			cout << endl;
		}
	}
	else if (lastCommand == 0) {
		cout << "행렬 1 :       행렬 2 :       결과 행렬 : " << '\n';
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				cout << matrix[i][j] << " ";

			}
			cout << "       ";
			for (int j = 0; j < 4; j++) {
				cout << matrix2[i][j] << " ";
			}
			cout << "       ";
			for (int j = 0; j < 4; j++) {
				cout << matrix3[i][j] << " ";
			}
			cout << endl;
		}
	}
}

void multiply(int matrix1[4][4], int matrix2[4][4], int result[4][4]) 
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				result[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
	


}

void add(int matrix1[4][4], int matrix2[4][4], int result[4][4]) 
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i][j] = 0;
			result[i][j] = matrix1[i][j] + matrix2[i][j];
		}
	}
}

void subtract(int matrix1[4][4], int matrix2[4][4], int result[4][4]) 
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i][j] = 0;
			result[i][j] = matrix1[i][j] - matrix2[i][j];
		}
	}
}

double Determinant(int matrix[4][4]) 
{
	double det = 1;

	// 1 양수 -1 음수, 행 교환시마다 *= -1 하면 됨
	int detsign = 1;

	double submatrix[4][4];

	// 복사 행렬 만들기
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			submatrix[i][j] = matrix[i][j];
		}
	}

	// 가우스 소거법 사용

	// i 열의 i 번째 인자를 이용해서 아래 행들의 첫 번째 인자를 0으로 만들기
	for (int i = 0; i < 4 - 1; ++i) {
		
		// 바꿔야 할 인자가 0이면 아래 행들 중 첫 인자가 0이 아닌 행과 교환
		if (submatrix[i][i] == 0) {
			int j = i + 1;

			bool isSwap = false;

			for (; j < 4; ++j) {
				// 바꿔야 할 인자가 0이 아닌 행을 찾으면 교환
				if (submatrix[j][i] != 0) {
					for (int k = 0; k < 4; ++k) {
						swap(submatrix[i][k], submatrix[j][k]);
					}
					detsign *= -1;
					isSwap = true;
					break;
				}
			}
			// 바꿀 수 있는 행이 없음으로 det 는 0이 나온다.
			if (isSwap == false) {
				return 0;
			}
		}
	
		for (int j = i + 1; j < 4; ++j) {
			double rate = submatrix[j][i] / submatrix[i][i];
			for (int k = i; k < 4; ++k) {
				submatrix[j][k] -= rate * submatrix[i][k];
			}
		}

	}// 여기까지 가우스 소거법 중.
	
	for (int i = 0; i < 4; ++i) {
		det *= submatrix[i][i];
	}

	det *= detsign;

	return det;
}

void transpose(int matrix[4][4])
{
	int submatrix[4][4];

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			submatrix[i][j] = matrix[j][i];
		}
	}
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matrix[i][j] = submatrix[i][j];
		}
	}
}

void subtractMin(int matrix[4][4], int eCount[4], bool* isE)
{
	// isE 가 가르키는 값이 false이면 각 행에서 최소값을 찾고
	// 그 값을 해당 행의 모든 인자에서 빼고 isE 를 true로 바꿈

	// isE 가 가르키는 값이 true면 eCount[i] 에 저장된 값만큼
	// i 행의 모든 인자에 더하고 isE 를 false로 바꿈

	if (*(isE) == false) {
		for (int i = 0; i < 4; ++i) {
			int min = matrix[i][0];
			for (int j = 1; j < 4; ++j) {
				if (matrix[i][j] < min) {
					min = matrix[i][j];
				}
			}
			eCount[i] = min;
			for (int j = 0; j < 4; ++j) {
				matrix[i][j] -= min;
			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				matrix[i][j] += eCount[i];
			}
		}
	}

	*(isE) = !(*(isE));

}

void addMax(int matrix[4][4], int fCount[4], bool* isF) 
{
	if (*isF == false) {
		for (int i = 0; i < 4; ++i) {
			fCount[i] = matrix[0][i];
			for (int j = 1; j < 4; ++j) {
				if (matrix[j][i] > fCount[i]) {
					fCount[i] = matrix[j][i];
				}
			}

			for (int j = 0; j < 4; ++j) {
				for (int k = 0; k < 4; ++k) {
					matrix[j][k] += fCount[k];
				}
			}

		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				matrix[j][i] -= fCount[i];
			}
		}
	}

	*(isF) = !(*(isF));

}

void addOne(int matrix[4][4])
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matrix[i][j] = (matrix[i][j] + 1) % 10;
		}
	}
}

void subtractOne(int matrix[4][4])
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matrix[i][j] = (matrix[i][j] + 9) % 10;
		}
	}
}

//---------------------------------------------------------------------------------------------------
int main()
//---------------------------------------------------------------------------------------------------
{
	int matrix1[4][4];
	int matrix2[4][4];
	int matrix3[4][4];
	int matrix4[4][4];

	set(matrix1, matrix2, matrix3, matrix4);

	char command{};

	// -1 : set (2번 행렬까지 출력), 
	// 0 : 곱, 덧, 빼기 (3번 행렬까지 출력), 
	// 1 : 전치행렬 (2번 행렬까지 출력)
	// 2 : 최소값 빼기 (3, 4번 행렬만 출력)
	// 3 : 최대값 더하기 (3, 4번 행렬만 출력)
	int lastCommand = -1;

	bool isE = false;
	bool isE2 = false;
	int eCount[4]{};
	int eCount2[4]{};

	bool isF = false;
	bool isF2 = false;
	int fCount[4]{};
	int fCount2[4]{};
	
	while (true) {
		
		show(matrix1, matrix2, matrix3, matrix4, lastCommand);

		cout << '\n' << '\n';

		cout << "명령어를 입력하세요 (m: 곱셈, a: 덧셈, d: 뺄셈, r: 행렬식, t: 전치행렬, e: 최소값 빼기, f: 최대값 더하기, +: +1, -: -1, s: 새로 설정, q: 종료) : ";
		
		cin >> command;

		if (command == 'm') {
			// 행렬 곱셈
			multiply(matrix1, matrix2, matrix3);
			lastCommand = 0;
		}
		else if (command == 'a') {
			// 행렬 덧셈
			add(matrix1, matrix2, matrix3);
			lastCommand = 0;
		}
		else if (command == 'd') {
			// 행렬 뺄셈
			subtract(matrix1, matrix2, matrix3);
			lastCommand = 0;
		}
		else if (command == 'r') {
			// 행렬식 계산
			double det1 = Determinant(matrix1);
			double det2 = Determinant(matrix2);
			lastCommand = -1;
			cout << '\n' << "행렬 1의 행렬식 : " << det1 << '\n' << '\n';
			cout << "행렬 2의 행렬식 : " << det2 << '\n' << '\n';
		}
		else if (command == 't') {
			// 전치행렬 계산
			transpose(matrix1);
			transpose(matrix2);
			double det1 = Determinant(matrix1);
			double det2 = Determinant(matrix2);
			lastCommand = -1;
			cout << '\n' << "행렬 1의 행렬식 : " << det1 << '\n' << '\n';
			cout << "행렬 2의 행렬식 : " << det2 << '\n' << '\n';
		}
		else if (command == 'e') {
			// 각 행에서 최소값 빼기
			subtractMin(matrix1, eCount, &isE);
			subtractMin(matrix2, eCount2, &isE2);
			lastCommand = -1;
		}
		else if (command == 'f') {
			// 각 열에서 최대값 더하기
			addMax(matrix1, fCount, &isF);
			addMax(matrix2, fCount2, &isF2);
			lastCommand = -1;
		}
		else if (command == '+') {
			// 모든 값에 +1
			addOne(matrix1);
			addOne(matrix2);
			lastCommand = -1;
		}
		else if (command == '-') {
			// 모든 값에 -1
			subtractOne(matrix1);
			subtractOne(matrix2);
			lastCommand = -1;
		}
		else if (command == 's') {
			set(matrix1, matrix2, matrix3, matrix4);
			lastCommand = -1;
		}
		else if (command == 'q') {
			break;
		}
	}
}