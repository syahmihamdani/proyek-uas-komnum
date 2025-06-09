// Proyek UAS Komputasi Numerik
// Syahmi Hamdani (2306220532)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_POINTS 100

typedef struct {
    double x[MAX_POINTS];
    double y[MAX_POINTS];
    double dd_table[MAX_POINTS][MAX_POINTS]; // Menyimpan data dd (divided difference)
    int n; // Jumlah titik data
} InterpolationData;

// Fungsi menghitung divided difference
void calculate_divided_differences(InterpolationData *data) {
    int i, j;
    
    // Inisialisasi kolom pertama (f[xi])
    for (i = 0; i < data->n; i++) {
        data->dd_table[i][0] = data->y[i];
    }
    
    // Menghitung dd orde lebih tinggi
    for (j = 1; j < data->n; j++) {
        for (i = 0; i < data->n - j; i++) {
            data->dd_table[i][j] = (data->dd_table[i+1][j-1] - data->dd_table[i][j-1]) / 
                                   (data->x[i+j] - data->x[i]);
        }
    }
}

// Fungsi mengevaluasi newton's interpolation di titik x
double newton_interpolate(InterpolationData *data, double x) {
    double result = data->dd_table[0][0];
    double term = 1.0;
    
    for (int i = 1; i < data->n; i++) {
        term *= (x - data->x[i-1]);
        result += data->dd_table[0][i] * term;
    }
    
    return result;
}

// Fungsi mencetak tabel dd 
void print_divided_difference_table(InterpolationData *data) {
    printf("\nTabel Divided Difference:\n");
    printf("%-8s %-10s", "x", "f[x]");
    
    for (int j = 1; j < data->n; j++) {
        printf(" %-12s", "f[x,...]");
    }
    printf("\n");
    printf("--------------------------------------------------------------------\n");
    
    for (int i = 0; i < data->n; i++) {
        printf("%-8.1f %-10.3f", data->x[i], data->dd_table[i][0]);
        for (int j = 1; j < data->n - i; j++) {
            printf(" %-12.6f", data->dd_table[i][j]);
        }
        printf("\n");
    }
}

// Fungsi menyimpan data awal ke CSV
void save_original_data_csv(InterpolationData *data, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot create file %s\n", filename);
        return;
    }
    
    fprintf(file, "jam,suhu\n");
    for (int i = 0; i < data->n; i++) {
        fprintf(file, "%.1f,%.1f\n", data->x[i], data->y[i]);
    }
    
    fclose(file);
    printf("Data awal tersimpan ke %s\n", filename);
}

// Fungsi menyimpan data hasil interpolasi ke CSV
void save_interpolated_data_csv(InterpolationData *data, const char *filename, double x_min, double x_max, int num_points) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot create file %s\n", filename);
        return;
    }
    
    fprintf(file, "jam,suhu_terinterpolasi\n");
    
    double step = (x_max - x_min) / (num_points - 1);
    for (int i = 0; i < num_points; i++) {
        double x = x_min + i * step;
        double y = newton_interpolate(data, x);
        fprintf(file, "%.2f,%.3f\n", x, y);
    }
    
    fclose(file);
    printf("Data interpolasi tersimpan ke %s\n", filename);
}

// Fungsi menyimpan tabel dd ke CSV
void save_dd_table_csv(InterpolationData *data, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot create file %s\n", filename);
        return;
    }
    
    // Header
    fprintf(file, "x,f[x]");
    for (int j = 1; j < data->n; j++) {
        fprintf(file, ",f[x_%d]", j);
    }
    fprintf(file, "\n");
    
    // Data
    for (int i = 0; i < data->n; i++) {
        fprintf(file, "%.1f,%.6f", data->x[i], data->dd_table[i][0]);
        for (int j = 1; j < data->n - i; j++) {
            fprintf(file, ",%.8f", data->dd_table[i][j]);
        }
        // Cell yang kosong diberi koma agar CSV konsisten
        for (int j = data->n - i; j < data->n; j++) {
            fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("Tabel Divided Difference tersimpan ke %s\n", filename);
}

// Fungsi menyimpan hasil analisis interpolasi
void save_analysis_results_csv(InterpolationData *data, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot create file %s\n", filename);
        return;
    }
    
    fprintf(file, "jam_test,suhu_terinterpolasi,tipe_analisis\n");
    
    // Titik-titik pengevaluasian
    double test_times[] = {6.5, 7.5, 8.5, 10.5, 11.5, 13.5, 14.5, 16.5, 17.5, 19.5, 20.5};
    int num_tests = sizeof(test_times) / sizeof(test_times[0]);
    
    for (int i = 0; i < num_tests; i++) {
        double temp = newton_interpolate(data, test_times[i]);
        fprintf(file, "%.1f,%.3f,titik_terinterpolasi\n", test_times[i], temp);
    }
    
    fclose(file);
    printf("Hasil analisis tersimpan ke %s\n", filename);
}

// Main function untuk menjalankan case study
void main_function(InterpolationData *data) {
    printf("================================================================\n");
    printf("        NEWTON'S DIVIDED DIFFERENCE INTERPOLATION\n");
    printf("     Case Study: Analisis Variasi Suhu Harian\n");
    printf("================================================================\n\n");
    
    printf("DESCRIPTION:\n");
    printf("- Periode Pengambilan Data: 6 AM sampai 9 PM (15 jam)\n");
    printf("- Kondisi cuaca: Cerah, cukup lembap\n");
    printf("- Tujuan: Membuat model kurva suhu untuk beberapa titik waktu dalam sehari\n\n");
    
    // Data jam (24h) dan suhu (celcius)
    double hours[] = {6, 9, 12, 15, 18, 21}; 
    double temps[] = {15.2, 18.7, 24.5, 28.3, 25.1, 20.8}; 
    
    data->n = 6;
    for (int i = 0; i < data->n; i++) {
        data->x[i] = hours[i];
        data->y[i] = temps[i];
    }
    
    printf("Data Temperatur Awal:\n");
    printf("Jam (24h)  |     Suhu (C)     | Periode\n");
    printf("-----------|------------------|------------------\n");
    for (int i = 0; i < data->n; i++) {
        char period[20];
        if (data->x[i] < 12) strcpy(period, "Pagi");
        else if (data->x[i] < 17) strcpy(period, "Siang");
        else strcpy(period, "Sore");
        
        printf("%-10.0f | %-16.1f | %s\n", data->x[i], data->y[i], period);
    }
    
    // Menghitung dd
    calculate_divided_differences(data);
    
    printf("\n");
    print_divided_difference_table(data);
    
    // Analisis Polinomial
    printf("\nAnalisis Polinomial:\n");
    printf("- Derajat/Degree: %d (n-1, dimana n = %d titik data)\n", data->n-1, data->n);
    printf("- Leading Coefficient: %.8f\n", data->dd_table[0][data->n-1]);
    printf("- Perilaku Polinomial: ");
    if (data->dd_table[0][data->n-1] > 0) {
        printf("Tren meningkat di boundaries\n");
    } else {
        printf("Tren menurun di boundaries\n");
    }
    
    // Hasil nilai terinterpolasi
    printf("\nNilai Suhu Terinterpolasi:\n");
    printf("Jam (24h) | Suhu Terinterpolasi (C) |\n");
    printf("-----------|------------------------|\n");
    
    double key_times[] = {7.5, 10.5, 13.5, 16.5, 19.5};
    
    for (int i = 0; i < 5; i++) {
        double temp = newton_interpolate(data, key_times[i]);
        printf("%-10.1f | %-22.2f |\n", key_times[i], temp);
    }
    
    // Interpretasi
    printf("\nInterpretasi:\n");
    double max_temp = 0, max_time = 0;
    double min_temp = 100, min_time = 0;
    
    // Mencari nilai maximum dan minimum dari suhu
    for (double t = 6.0; t <= 21.0; t += 0.1) {
        double temp = newton_interpolate(data, t);
        if (temp > max_temp) {
            max_temp = temp;
            max_time = t;
        }
        if (temp < min_temp) {
            min_temp = temp;
            min_time = t;
        }
    }
    
    printf("- Suhu puncak: %.2fC di perkiraan jam %.1f\n", max_temp, max_time);
    printf("- Suhu minimum: %.2fC di perkiraan jam %.1f\n", min_temp, min_time);
    printf("- Jarak suhu (max - min): %.2fC\n", max_temp - min_temp);
    printf("- Laju pemanasan (6-12 AM): %.2fC/jam\n", (newton_interpolate(data, 12) - newton_interpolate(data, 6)) / 6);
    printf("- Laju pendinginan (6-9 PM): %.2fC/jam\n\n", (newton_interpolate(data, 21) - newton_interpolate(data, 18)) / 3);
    
    // Menyimpan semua file CSV
    save_original_data_csv(data, "original_temperature_data.csv");
    save_interpolated_data_csv(data, "interpolated_temperature_data.csv", 6.0, 21.0, 150);
    save_dd_table_csv(data, "divided_difference_table.csv");
    save_analysis_results_csv(data, "temperature_analysis_results.csv");
    
}

int main() {
    InterpolationData data;
    
    main_function(&data);
    
    return 0;
}   