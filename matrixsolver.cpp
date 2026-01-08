#include "MatrixSolver.h"
#include <cmath>
#include <stdexcept>

namespace
{
    const double EPSILON = 1e-10;

    inline bool isZero(double x)
    {
        return std::fabs(x) < EPSILON;
    }
}

// Решение системы с фиксированной последней переменной
QVector<double> MatrixSolver::solveWithFixedLastVariable(
    const QVector<QVector<double>>& A,
    const QVector<double>& B,
    double fixedValue)
{
    if (A.isEmpty() || B.isEmpty() || A[0].isEmpty())
    {
        throw std::invalid_argument("Пустая матрица");
    }

    int rows = A.size();
    int cols = A[0].size();

    // Проверка размеров
    for (const auto& row : A)
    {
        if (row.size() != cols)
        {
            throw std::invalid_argument("Разные размеры строк");
        }
    }
    if (B.size() != rows)
    {
        throw std::invalid_argument("Размеры не совпадают");
    }

    // Новая система без последнего столбца
    QVector<QVector<double>> A_prime(rows, QVector<double>(cols - 1));
    QVector<double> B_prime(rows);

    for (int i = 0; i < rows; ++i)
    {
        // Копируем все кроме последнего столбца
        for (int j = 0; j < cols - 1; ++j)
        {
            A_prime[i][j] = A[i][j];
        }
        // Последний столбец переносим в правую часть
        B_prime[i] = B[i] - A[i][cols - 1] * fixedValue;
    }

    // Решаем систему
    QVector<double> solution = solveSystem(A_prime, B_prime);
    solution.append(fixedValue); // Добавляем фиксированное значение

    return solution;
}

// Решение системы методом Гаусса
QVector<double> MatrixSolver::solveSystem(
    const QVector<QVector<double>>& A,
    const QVector<double>& B)
{
    int rows = A.size();
    int cols = A[0].size();

    // Расширенная матрица [A|B]
    QVector<QVector<double>> augmented = A;
    for (int i = 0; i < rows; ++i)
    {
        augmented[i].append(B[i]);
    }

    // Прямой ход
    int rank = 0;
    for (int col = 0; col < cols && rank < rows; ++col)
    {
        // Поиск ненулевого элемента
        int pivot = rank;
        while (pivot < rows && isZero(augmented[pivot][col]))
        {
            ++pivot;
        }

        if (pivot == rows) continue; // Весь столбец нулевой

        // Меняем строки местами
        if (pivot != rank)
        {
            augmented.swapItemsAt(rank, pivot);
        }

        // Делим строку на диагональный элемент
        double div = augmented[rank][col];
        for (int j = col; j <= cols; ++j)
        {
            augmented[rank][j] /= div;
        }

        // Обнуляем столбец в других строках
        for (int i = 0; i < rows; ++i)
        {
            if (i == rank || isZero(augmented[i][col])) continue;

            double factor = augmented[i][col];
            for (int j = col; j <= cols; ++j)
            {
                augmented[i][j] -= factor * augmented[rank][j];
            }
        }
        ++rank;
    }

    // Проверка на совместность
    for (int i = rank; i < rows; ++i)
    {
        if (!isZero(augmented[i][cols]))
        {
            throw std::invalid_argument("Система несовместна");
        }
    }

    // Обратный ход
    QVector<double> solution(cols, 0.0);
    for (int i = rank - 1; i >= 0; --i)
    {
        // Находим ведущий столбец
        int lead_col = 0;
        while (lead_col < cols && isZero(augmented[i][lead_col]))
        {
            ++lead_col;
        }

        if (lead_col >= cols) continue;

        // Вычисляем переменную
        solution[lead_col] = augmented[i][cols];
        for (int j = lead_col + 1; j < cols; ++j)
        {
            solution[lead_col] -= augmented[i][j] * solution[j];
        }
    }

    return solution;
}
