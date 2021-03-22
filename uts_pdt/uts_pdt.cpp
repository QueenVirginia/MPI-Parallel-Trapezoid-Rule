//Nama      : Queen Virginia J. T.
//NIM       : 00000029272
//No.Absen  : 12

#include <Windows.h>
#include <stdio.h>
#include <mpi.h>

//persamaan yang akan di integalkan
double f(double x) {
    double h = 3 * x * x + 4 * x + 12;
    return(h);
}

//mencari nilai integral dengan metode trapezoidal
double trapezoidal(double bawah, double atas, long n) {
    int i;
    double xi, yi;
    double y = 0;
    double h = (atas - bawah) / n;

    y = 0.5 * h * (f(atas) + f(bawah));
    for (i = 1; i < n; i++) {
        xi = bawah + i * h;
        yi = f(xi);
        y += h * yi;
    }
    return y;
}

int main(int argc, char* argv[])
{
    int my_rank, p;
    long local_n;
    double BatasBawah = 10;
    double BatasAtas = 50;
    long N = 40000000; //jumlah iterasi
    double Bawah, Atas, h, value, start, finish, waktu;

    //memulai program paralel
    MPI_Init(&argc, &argv);
    //mendaoatkan rank proses
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    //meneumakan berapa proses yang digunakan
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    //memulai waktu
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    h = (BatasAtas - BatasBawah) / N; //menghitung panjang pada setiap trapezoid
    local_n = N / p; //nilai trapezoid per proses

    //panjang dari setiap proses interval dari integral 
    Bawah = BatasBawah + my_rank * local_n * h;
    Atas = Bawah + local_n * h;
    double y = trapezoidal(Bawah, Atas, local_n);

    if (my_rank != 0)
    {
        //mengirim pesan ke proses penerima
        MPI_Send(&y, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        value = y;
        for (int i = 1; i < p; i++)
        {
            //menerima pesan dari proses pengirim
            MPI_Recv(&y, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            value += y;
        }

        //memberhentikan waktu
        finish = MPI_Wtime();
        waktu = finish - start;

        //menampilkan hasil
        printf("Nilai Integral = %f, waktu (Tp) = %lf second\n", value, waktu);
    }

    //untuk menyelesaikan program paralel
    MPI_Finalize();

    return 0;
}
