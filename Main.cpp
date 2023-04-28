#include <immintrin.h>
#include <iostream>
#include <chrono>
#include <random>

using namespace std;
//ниже представлены 
int len = 16384;
long double sclFull=0;
long double vecFull=0;
//дл€ доступа к функци€м умножени€ объ€вим их перед функцией main
void Cmp(double** matrix_v, double** matrix_c);
void Scl(double** matrix_a, double** matrix_b, double** matrix_c);
void Vec(double** matrix_a, double** matrix_b, double** matrix_c);

int main()
{
	//создаЄм матрицы
	double** Matrix_vec = new double* [len];
	double** Matrix_sc = new double* [len];
	double** Matrix_a = new double* [len];
	double** Matrix_b = new double* [len];

	//заполн€ем матрицы случайными числами
	for (int i = 0;i < len;i++)
	{
		Matrix_vec[i] = new double[len];
		Matrix_sc[i] = new double[len];
		Matrix_a[i] = new double[len];
		Matrix_b[i] = new double[len];

		for (int j = 0;j < len;j++)
		{
			Matrix_vec[i][j] = 0;
			Matrix_sc[i][j] = 0;
			Matrix_a[i][j] = 1.0 + rand() % 400;
			Matrix_b[i][j] = 1.0 + rand() % 400;
		}
	}

	//считаем врем€ дл€ скал€рного метода
	for (int i=0;i<10;i++)
	{
		//засекаем врем€ старта 
		auto scl_start = chrono::high_resolution_clock::now();
		//вызываем скал€рное сложение
		Scl(Matrix_a, Matrix_b, Matrix_sc);
		//засекаем врем€ окончани€
		auto scl_end = chrono::high_resolution_clock::now();
		//вычисл€ем продолжительность в секундах
		chrono::duration<double> scl_dur = (scl_end - scl_start) * 1000;
		sclFull += scl_dur.count();
	}

	//начинаем считать врем€ дл€ векторного рассчЄта
	for (int i = 0;i < 10;i++)
	{
		//засекаем врем€ старта отсчЄта
		auto vec_start = chrono::high_resolution_clock::now();
		//вызываем векторное сложение
		Vec(Matrix_a, Matrix_b, Matrix_vec);
		//засекаем врем€ окончани€
		auto vec_end = chrono::high_resolution_clock::now();
		//вычисл€ем длительность операции и записываем в переменную
		chrono::duration<double> vec_dur = (vec_end - vec_start) * 1000;
		vecFull += vec_dur.count();
	}
	//вызываем функцию сравнени€ матриц
	Cmp(Matrix_sc, Matrix_vec);
	//выводим врем€ работы методов 
	cout << "Scalar time: " << sclFull/10 << endl;
	cout << "Vector time: " << vecFull/10 << endl;
	//зачищаем пам€ть
	for (int i = 0; i < len;i++)
	{
		delete[] Matrix_vec[i];
		delete[] Matrix_sc[i];
		delete[] Matrix_a[i];
		delete[] Matrix_b[i];
	}
	delete[] Matrix_vec;
	delete[] Matrix_sc;
	delete[] Matrix_a;
	delete[] Matrix_b;
}

//функци€ дл€ скал€рного(стандартного) сложени€
void Scl(double** matrix_a, double** matrix_b, double** matrix_c)
{
	for (int i = 0;i < len;i++)
	{
		for (int j = 0;j < len;j++)
		{
			matrix_c[i][j] = matrix_a[i][j] + matrix_b[i][j];
		}
	}
}

//функци€ векторного вычислени€ суммы
void Vec(double** matrix_a, double** matrix_b, double** matrix_c)
{
	//__m256d последн€€ буква d отсылает на тип данных double
	__m256d sum;
	__m256d vector_a_1;
	__m256d vector_b_1;
	for (int i = 0;i < len;i++)
	{
		for (int j = 0;j < len;j+=4)
		{
			//подгружаем значени€ в векторы
			vector_a_1 = _mm256_load_pd(&matrix_a[i][j]);
			vector_b_1 = _mm256_load_pd(&matrix_b[i][j]);
			//выполн€ем инструкцию сложени€ 
			sum = _mm256_add_pd(vector_a_1,vector_b_1);
			//сгружаем значени€ в пам€ть по адресу
			_mm256_storeu_pd(&matrix_c[i][j], sum);
		}
	}
}

//функци€ сравнени€ матриц дл€ проверки результатов
void Cmp(double** matrix_v, double** matrix_c)
{
	bool isEqual = true;
	for (int i = 0;i < len;i++)
	{
		for (int j = 0;j < len;j++)
		{
			if (matrix_v[i][j] != matrix_c[i][j])
			{
				isEqual = false;
				break;
			}
		}
		if (isEqual == false)
			break;
	}
	if (isEqual)
	{
		cout << "All right, matrixs are equal \n";
	}
	else
	{
		cout << "Oh my god, matrixs aren't equal \n";
	}
}