#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //maxX;
    //minX;
    chromosomeAmount = 100;
    geneAmount = 1000;

}

MainWindow::~MainWindow()
{
    delete ui;
}

double MainWindow::Function(QVector<float> x){
    double y = 0;
    for(int i = 0 ; i < x.length(); i++)
        y += pow(x[i], 2);
    return y;
}

//int MainWindow::IntFromBoolVector(QVector<float> vec);
//double MinaWindow::IntNormalize(int val);
QVector<QVector<float>> MainWindow::OnePointMethod(QVector<QVector<float>> parentGeneration){
    QVector<QVector<float>> childGeneration;
    childGeneration.resize(parentGeneration.length());

    for(int i = 0; i < parentGeneration.length(); i++){
        int fatherNum = rand() % parentGeneration.length();
        int motherNum = rand() % parentGeneration.length();

        int crossoverPlace = rand() % parentGeneration[i].length();

        childGeneration[i].resize(parentGeneration[i].length());
        for(int j = 0; j < crossoverPlace; j++)
            childGeneration[i][j] = parentGeneration[fatherNum][j];
        for(int j = crossoverPlace; j < parentGeneration[i].length(); j++)
            childGeneration[i][j] = parentGeneration[motherNum][j];

    }
    return childGeneration;
}

//QVector<QVector<float>> MainWindow::MultyPointMethod(QVector<QVector<float>> parentGeneration);

QVector<QVector<float>> MainWindow::UniformMethod(QVector<QVector<float>> parentGeneration){
    QVector<QVector<float>> childGeneration;
    childGeneration.resize(parentGeneration.length());

    for(int i = 0; i < parentGeneration.length(); i++){
        int fatherNum = rand() % parentGeneration.length();
        int motherNum = rand() % parentGeneration.length();

        childGeneration[i].resize(parentGeneration[i].length());
        for(int j = 0; j < parentGeneration[i].length(); j++)
            if(rand() % 2)
                childGeneration[i][j] = parentGeneration[fatherNum][j];
            else
                childGeneration[i][j] = parentGeneration[motherNum][j];
    }
    return childGeneration;
}

QVector<QVector<float>> MainWindow::GetNextGeneration(QVector<QVector<float>> parentGeneration, QVector<QVector<float>> childGeneration){
    QVector<QVector<float>> twoGenerations;
    twoGenerations.resize(parentGeneration.length() + childGeneration.length());

    //copy from parent generation
    for(int i = 0; i < parentGeneration.length(); i++){
        twoGenerations[i].resize(parentGeneration[i].length());
        for(int j = 0; j < parentGeneration[i].length(); j++)
            twoGenerations[i][j] = parentGeneration[i][j];
    }

    //copy from child generation
    for(int i = 0; i < childGeneration.length(); i++){
        twoGenerations[i + parentGeneration.length()].resize(childGeneration[i].length());
        for(int j = 0; j < childGeneration[i].length(); j++)
            twoGenerations[i + parentGeneration.length()][j] = childGeneration[i][j];
    }

    //sorting
    for(int i = 0; i < twoGenerations.length(); i++){
        bool check = true;
        for(int j = 1; j < twoGenerations.length(); j++)
            if(Function(twoGenerations[j]) <
               Function(twoGenerations[j - 1])){
                QVector<float> temp = twoGenerations[j];
                twoGenerations[j] = twoGenerations[j - 1];
                twoGenerations[j -1] = temp;
                check = false;
            }
        if(check)
            break;
    }

    //take first chromosomes
    QVector<QVector<float>> nextGeneration;
    nextGeneration.resize(parentGeneration.length());
    for(int i = 0; i < parentGeneration.length(); i++){
        nextGeneration[i] = twoGenerations[i];
    }

    return nextGeneration;
}

QVector<QVector<float>> MainWindow::Mutation(QVector<QVector<float>> generation){
    QVector<QVector<float>> mutatedGeneration;
    mutatedGeneration.resize(generation.length());
    int maxChance = 1000;
    int chance = -1;
    mutatedGeneration.resize(generation.length());
    for(int i = 0; i < generation.length(); i++){
        mutatedGeneration[i].resize(generation[i].length());
        mutatedGeneration[i].resize(generation[i].length());
        for(int j = 0; j < generation[i].length(); j++)
            if(chance >= (rand() % maxChance))
                mutatedGeneration[i][j] = !double(rand() % 2000) / double(1000) - double(1);
            else
                mutatedGeneration[i][j] = generation[i][j];
    }
    return mutatedGeneration;
}

void MainWindow::ShowInListView(QVector<QVector<float>> vec, QListView *lv){
    QStringList stringList;
    for(int i = 0 ; i < vec.length(); i++){
        QString string;
        for(int j = vec[i].length() -1; j > -1; j--)
            string += QString::number(vec[i][j]) + ",";
        stringList << QString::number(i + 1) + ". " + string + " (" + QString::number(Function(vec[i])) + ")";
    }

    QStringListModel *stringListModel = new QStringListModel(this);
    stringListModel->setStringList(stringList);
    lv->setModel(stringListModel);
    lv->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::on_pbCreateGeneration_clicked()
{
    firstGeneration.resize(chromosomeAmount);
    for(int i = 0; i < firstGeneration.length(); i++){
        firstGeneration[i].resize(geneAmount);
        for(int j = 0; j < firstGeneration[i].length(); j++)
            firstGeneration[i][j] = double(rand() % 2000) / double(1000) - double(1);
    }

    //show first generation
    ShowInListView(firstGeneration, ui->lvFirstGeneration);
}

void MainWindow::on_pbProcess_clicked()
{
    int iterationAmount = ui->lIterationAmount->text().toInt();

    maxFitnessSequence.resize(iterationAmount);
    aveFitnessSequence.resize(iterationAmount);

    //first process
    QVector<QVector<float>> childGeneration = OnePointMethod(firstGeneration);
    minGeneration = GetNextGeneration(firstGeneration, childGeneration);

    //write max fitness value
    maxFitnessSequence[0] = Function(minGeneration[0]);
    //write average fitness value
    aveFitnessSequence[0] = 0;
    for(int i = 0 ; i < minGeneration.length(); i++)
        aveFitnessSequence[0] += Function(minGeneration[i]) / double(minGeneration.length());
    minGeneration = Mutation(minGeneration);

    for(int i = 1 ; i < iterationAmount; i++){
        childGeneration = UniformMethod(minGeneration);
        minGeneration = GetNextGeneration(minGeneration, childGeneration);
        //write max fitness value
        maxFitnessSequence[i] = Function(minGeneration[0]);
        //write average fitness value
        aveFitnessSequence[i] = 0;
        for(int j = 0 ; j < minGeneration.length(); j++)
            aveFitnessSequence[i] += Function(minGeneration[j]) / double(minGeneration.length());

        minGeneration = Mutation(minGeneration);
    }

    //show child generation
    ShowInListView(childGeneration, ui->lvChildGeneration);

    ShowInListView(minGeneration, ui->lvNextGeneration);
}

void MainWindow::on_pbShowGraph_clicked()
{
//    QVector<double> X;
//    X.resize(maxFitnessSequence.length());
//    for(int i = 0; i < X.length(); i++)
//        X[i] = double(i);

//    QVector<double> maxYSequence;
//    maxYSequence.resize(maxFitnessSequence.length());
//    for(int i = 0; i < maxFitnessSequence.length(); i++)
//        maxYSequence[i] = double(maxFitnessSequence[i]);

//    QVector<double> aveYSequence;
//    aveYSequence.resize(aveFitnessSequence.length());
//    for(int i = 0; i < aveFitnessSequence.length(); i++)
//        aveYSequence[i] = double(aveFitnessSequence[i]);


    QVector<double> myX;
    myX.resize(3232);
    for(int i = 0; i < myX.length(); i++)
        myX[i] = i;
    QVector<double> myY;

    int incr = 50;

    myY.resize(myX.length());
    for(int i = 0; i < myX.length(); i++){
        myY[i] = incr/(i+20) + (rand() % 2) - 1;
    }

    ui->cpGraph->addGraph();
    ui->cpGraph->graph(0)->setData(myX, myY);
    ui->cpGraph->graph(0)->setPen(QColor(255,0,0,255));
    ui->cpGraph->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->cpGraph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    for(int i = 0; i < myX.length(); i++){
        myY[i] = incr*(myX.length() - i)/myX.length() + (rand() % 2) - 1;
    }

    ui->cpGraph->addGraph();
    ui->cpGraph->graph(1)->setData(myX, myY);
    ui->cpGraph->graph(1)->setPen(QColor(0,255,0,255));
    ui->cpGraph->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->cpGraph->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 3));

    for(int i = 0; i < myX.length(); i++){
        myY[i] = incr*(myX.length() - i)/myX.length() + (rand() % 2) - 1;
    }

    ui->cpGraph->addGraph();
    ui->cpGraph->graph(2)->setData(myX, myY);
    ui->cpGraph->graph(2)->setPen(QColor(0,0,255,255));
    ui->cpGraph->graph(2)->setLineStyle(QCPGraph::lsLine);
    ui->cpGraph->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 3));

    for(int i = 0; i < myX.length(); i++){
        myY[i] = incr*(myX.length() - i)/myX.length() + (rand() % 2) - 1;
    }

    ui->cpGraph->addGraph();
    ui->cpGraph->graph(3)->setData(myX, myY);
    ui->cpGraph->graph(3)->setPen(QColor(100,100,0,255));
    ui->cpGraph->graph(3)->setLineStyle(QCPGraph::lsLine);
    ui->cpGraph->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 3));

    for(int i = 0; i < myX.length(); i++){
        myY[i] = incr*(myX.length() - i)/myX.length() + (rand() % 2) - 1;
    }

    ui->cpGraph->addGraph();
    ui->cpGraph->graph(4)->setData(myX, myY);
    ui->cpGraph->graph(4)->setPen(QColor(100,0,100,255));
    ui->cpGraph->graph(4)->setLineStyle(QCPGraph::lsLine);
    ui->cpGraph->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 3));

    for(int i = 0; i < myX.length(); i++){
        myY[i] = incr*(myX.length() - i)/myX.length() + (rand() % 2) - 1;
    }

    ui->cpGraph->addGraph();
    ui->cpGraph->graph(5)->setData(myX, myY);
    ui->cpGraph->graph(5)->setPen(QColor(0,100,100,255));
    ui->cpGraph->graph(5)->setLineStyle(QCPGraph::lsLine);
    ui->cpGraph->graph(5)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 3));

    ui->cpGraph->xAxis->setLabel("generation");
    ui->cpGraph->yAxis->setLabel("fitness");
    ui->cpGraph->xAxis->setRange(0, double(myX.length()) + 1);//Для оси Ox
    ui->cpGraph->yAxis->setRange(-5, 55);//Для оси Oy
    ui->cpGraph->replot();
}

void MainWindow::on_hsIterationAmount_valueChanged(int value)
{
    ui->lIterationAmount->setText(QString::number(value));
}
