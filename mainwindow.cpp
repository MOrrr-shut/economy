#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "matrixsolver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->line_error_product->setVisible(true);
    ui->table_kon->setVisible(false);

    // Дефолтные значения
    number_people = 20000000;
    number_productov = 5;
    number_otrasl = 3;
    cost_trud = 100;
    trud_hour = 8;

    ui->line_people->setText(QString::number(number_people));
    ui->line_firm->setText(QString::number(number_productov));
    ui->line_otrasl->setText(QString::number(number_otrasl));
    ui->line_trud->setText(QString::number(cost_trud));
    ui->line_trud_hour->setText(QString::number(trud_hour));

    // Таблица подстраивается под контент
    ui->table_price->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_price->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tab_main_currentChanged(int index)
{
    // Расчет итогового труда
    double line_trud_itog = 0.0;
    bool data_ok = true;
    int lastColumn = ui->table_mop->columnCount() - 1;

    for (int row = 0; row < ui->table_mop->rowCount(); ++row)
    {
        QTableWidgetItem* mopItem = ui->table_mop->item(row, lastColumn);
        QTableWidgetItem* konItem = ui->table_kon->item(1, row);

        if (!mopItem || !konItem || mopItem->text().isEmpty() || konItem->text().isEmpty())
        {
            data_ok = false;
            break;
        }

        bool ok1, ok2;
        double mopValue = mopItem->text().toDouble(&ok1);
        double konValue = konItem->text().toDouble(&ok2);

        if (!ok1 || !ok2)
        {
            data_ok = false;
            break;
        }

        line_trud_itog += mopValue * konValue;
    }

    if (data_ok)
    {
        ui->line_trud_itog->setText(QString::number(line_trud_itog, 'f', 2));
    }
    else
    {
        ui->line_trud_itog->setText("Недостаточно данных");
    }

    // Расчет количества людей
    double count_people = line_trud_itog / trud_hour;
    if (qFuzzyCompare(count_people, std::round(count_people)))
    {
        ui->line_nalog_all->setText(QString::number(static_cast<int>(count_people)));
    }
    else
    {
        ui->line_nalog_all->setText(QString::number(static_cast<int>(count_people + 1)));
    }

    // Заполнение главной таблицы
    int totalColsRows = number_productov * 2 + number_otrasl * 2 + 2;
    ui->table_main->setColumnCount(totalColsRows);
    ui->table_main->setRowCount(totalColsRows);

    QStringList headers;
    for (int i = 0; i < totalColsRows; ++i)
    {
        if (i < number_productov)
            headers << QString("Фирма%1").arg(i+1);
        if (number_productov <= i && i < number_productov*2)
            headers << QString("Работник%1").arg(i-number_productov+1);
        if (i == number_productov*2 + 1)
            headers << QString("НД");
        if (i == number_productov*2 + 2)
            headers << QString("ГОС");
        if (number_productov*2 + 2 <= i && i < number_productov*2 + 2 + number_otrasl)
            headers << QString("ЮрЛ%1").arg(i+1 - number_productov*2 - 2);
        if (number_productov*2 + 2 + number_otrasl <= i && i < totalColsRows)
            headers << QString("ФизЛ%1").arg(i+1 - number_productov*2 - 2 - number_otrasl);
    }
    ui->table_main->setHorizontalHeaderLabels(headers);
    ui->table_main->setVerticalHeaderLabels(headers);

    // Цены продуктов
    QVector<double> prices;
    for (int col = 0; col < number_productov; ++col)
    {
        QTableWidgetItem* item = ui->table_price->item(0, col);
        prices.append(item ? item->text().toDouble() : 0.0);
    }

    // Потребности
    QVector<double> needs;
    for (int col = 0; col < number_productov; ++col)
    {
        QTableWidgetItem* item = ui->table_kon->item(0, col);
        needs.append(item ? item->text().toDouble() : 0.0);
    }

    // Заполнение для рабочих (диагональ)
    for (int workerCol = number_productov; workerCol < number_productov * 2; ++workerCol)
    {
        double sum = 0.0;
        for (int product = 0; product < number_productov; ++product)
        {
            sum += prices[product] * needs[product];
        }

        int row = workerCol - number_productov;
        if (row < number_productov)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(sum, 'f', 2));
            item->setTextAlignment(Qt::AlignCenter);
            ui->table_main->setItem(row, workerCol, item);
        }
    }

    // Заполнение данных МОП
    for (int i = 0; i < number_productov; ++i)
    {
        for (int j = 0; j < number_productov; ++j)
        {
            QTableWidgetItem* mopItem = ui->table_mop->item(i, j);
            QTableWidgetItem* priceItem = ui->table_price->item(0, j);

            if (mopItem && priceItem)
            {
                double mopValue = mopItem->text().toDouble();
                double priceValue = priceItem->text().toDouble();
                double result = mopValue * priceValue;

                QTableWidgetItem* item = new QTableWidgetItem(QString::number(result));
                ui->table_main->setItem(i, j, item);
            }
            else
            {
                ui->table_main->setItem(i, j, new QTableWidgetItem(" "));
            }
        }
    }

    // Настройка таблицы МОП при переключении на вкладку 2
    if (index == 2)
    {
        ui->table_mop->setColumnCount(number_productov);
        ui->table_mop->setRowCount(number_productov - 1);

        QStringList mopHeaders;
        for (int i = 0; i < number_productov; ++i)
        {
            if (i == number_productov - 1)
            {
                mopHeaders << QString("Труд (ч*ч)");
                continue;
            }
            mopHeaders << QString("Фирма %1").arg(i+1);
        }
        ui->table_mop->setHorizontalHeaderLabels(mopHeaders);
        ui->table_mop->setVerticalHeaderLabels(mopHeaders);
        ui->table_mop->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->table_mop->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
}

/////////////////////////////////////////////
// Изменение данных
/////////////////////////////////////////////

void MainWindow::on_line_people_textChanged(const QString &arg1)
{
    number_people = arg1.toInt();
}

void MainWindow::on_line_otrasl_textChanged(const QString &arg1)
{
    number_otrasl = arg1.toInt();
}

void MainWindow::on_line_firm_textChanged(const QString &arg1)
{
    number_productov = arg1.toInt();

    ui->table_kon->setVisible(true);
    ui->line_error_product->setVisible(false);

    // Настройка таблицы КОН
    ui->table_kon->setColumnCount(number_productov + number_otrasl - 1);
    ui->table_kon->setRowCount(2);

    QStringList headers;
    for (int i = 0; i < number_productov - 1; ++i)
    {
        headers << QString("Продукт фирмы %1").arg(i+1);
    }
    for (int i = 0; i < number_otrasl; ++i)
    {
        headers << QString("Продукт отрасли %1").arg(i+1);
    }

    ui->table_kon->setHorizontalHeaderLabels(headers);
    ui->table_kon->setVerticalHeaderLabels(QStringList() << "1 человек" << "Население");
    ui->table_kon->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_kon->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/////////////////////////////////////////////
// Расчет потребностей на население
/////////////////////////////////////////////

void MainWindow::on_table_kon_itemChanged(QTableWidgetItem *item)
{
    item->setTextAlignment(Qt::AlignCenter);

    if (item->row() == 0) // Первая строка (на человека)
    {
        bool ok;
        double value = item->text().toDouble(&ok);

        if (ok && number_people > 0)
        {
            double result = value * number_people;
            QTableWidgetItem *below = new QTableWidgetItem(QString::number(result, 'f', 2));
            below->setTextAlignment(Qt::AlignCenter);
            ui->table_kon->setItem(1, item->column(), below);
        }
        else
        {
            QTableWidgetItem* below = ui->table_kon->item(1, item->column());
            if (below) below->setText("");
        }
    }
}

/////////////////////////////////////////////
// Кнопка расчета МОП
/////////////////////////////////////////////

void MainWindow::on_pushButton_clicked()
{
    QTableWidget* tableMop = ui->table_mop;
    int rows = tableMop->rowCount();
    int cols = tableMop->columnCount();

    if (rows < 1 || cols < 2)
    {
        QMessageBox::warning(this, "Ошибка", "Недостаточно данных");
        return;
    }

    // Формируем матрицу A
    QVector<QVector<double>> A;
    QVector<double> B(rows, 0.0);

    for (int row = 0; row < rows; ++row)
    {
        QVector<double> rowData;
        bool rowHasData = false;

        for (int col = 0; col < cols; ++col)
        {
            double value = 0.0;
            QTableWidgetItem* item = tableMop->item(row, col);

            if (item)
            {
                QString text = item->text().trimmed();
                if (!text.isEmpty())
                {
                    bool ok;
                    value = text.toDouble(&ok);
                    if (!ok) value = 0.0;
                }
            }

            // Главная диагональ = -1
            if (row == col) value = -1.0;

            rowData.append(value);
            if (value != 0.0) rowHasData = true;
        }

        if (rowHasData) A.append(rowData);
    }

    if (A.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Нет данных для расчета");
        return;
    }

    // Фиксированное значение xₙ (труд)
    bool ok;
    double fixedValue = ui->line_trud->text().toDouble(&ok);
    if (!ok || fixedValue <= 0)
    {
        QMessageBox::warning(this, "Ошибка", "Некорректная стоимость труда");
        return;
    }

    // Решение системы
    QVector<double> X;
    try
    {
        X = MatrixSolver::solveWithFixedLastVariable(A, B, fixedValue);
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось решить систему");
        return;
    }

    if (X.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Нет решения");
        return;
    }

    // Вывод результатов в таблицу цен
    QTableWidget* tablePrice = ui->table_price;
    tablePrice->setRowCount(1);
    tablePrice->setColumnCount(X.size());

    for (int i = 0; i < X.size(); ++i)
    {
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(X[i], 'f', 2));
        item->setTextAlignment(Qt::AlignCenter);
        tablePrice->setItem(0, i, item);
    }

    QStringList headers;
    for (int i = 0; i < X.size(); ++i)
    {
        headers << QString("Цена %1").arg(i+1);
    }
    tablePrice->setHorizontalHeaderLabels(headers);
}
