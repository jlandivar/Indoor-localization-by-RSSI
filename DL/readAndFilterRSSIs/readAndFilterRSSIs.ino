#include "ESP8266WiFi.h"
#include <Wire.h>

#define CHANNEL 11
#define tamVector 10
#define SDA_PIN 0
#define SCL_PIN 2
const int16_t I2C_SLAVE = 0x08;

int vectorAP0[tamVector];
int vectorAP1[tamVector];
int vectorAP2[tamVector];
byte vectorDistancias[3];

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  Wire.begin(SDA_PIN, SCL_PIN, I2C_SLAVE); // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  
  Serial.println("Setup done");
}

void loop() {
  //
  int n = WiFi.scanNetworks(false, false, CHANNEL, NULL);
  for (int i = 0; i < n; ++i) {
    if (WiFi.SSID(i).equals("AP0")){
      vectorDistancias[0] = getDistanceFromAP(i, vectorAP0);
    } 
    else if (WiFi.SSID(i).equals("AP1")){
      vectorDistancias[1] = getDistanceFromAP(i, vectorAP1);
    } 
    else if (WiFi.SSID(i).equals("AP2")){
      vectorDistancias[2] = getDistanceFromAP(i, vectorAP2);
    }
  }
}

double getDistanceFromAP(int i, int vectorRSSI[]){
  for (int pos = tamVector-1; pos > 0; pos--) vectorRSSI[pos] = vectorRSSI[pos-1]; //se desplazan los valores anteriores
  vectorRSSI[0] = WiFi.RSSI(i); //se agrega el nuevo valor
  
  int vectorOrdenado[tamVector];
  for (int pos = 0; pos < tamVector; pos++) vectorOrdenado[pos] = vectorRSSI[pos]; //Se copia el vector, para no alterar el original
  quickSort(vectorOrdenado, 0, sizeof(vectorRSSI)/sizeof(vectorRSSI[0])-1); //Se ordena el nuevo vector
  
  double posQ1 = 0.25*(tamVector+1); 
  double Q1 = vectorOrdenado[(int)posQ1]+(vectorOrdenado[(int)posQ1+1]-vectorOrdenado[(int)posQ1])*(posQ1 - (int)posQ1);
  double posQ3 = 0.75*(tamVector+1);
  double Q3 = vectorOrdenado[(int)posQ3]+(vectorOrdenado[(int)posQ3+1]-vectorOrdenado[(int)posQ3])*(posQ3 - (int)posQ3);
  
  double RI = Q3-Q1;
  double cercaInferior = Q1-1.5*RI;
  double cercaSuperior = Q3+1.5*RI;

  double acumulador;
  double datosNoAberrantes;
  for (int pos = 0; pos < tamVector; pos++){
    if (vectorRSSI[pos] > cercaInferior && vectorRSSI[pos] < cercaSuperior){ //Para calcular la media, solo se usan los datos no aberrantes
      acumulador += vectorRSSI[pos];
      datosNoAberrantes++;
    }
  }
  
  double mediaCortada = acumulador/datosNoAberrantes;
  return 0.0000248*pow(mediaCortada,4)+0.0020628*pow(mediaCortada,3)+0.0619571*pow(mediaCortada,2)+0.5168627*mediaCortada+2.1085110;
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  //printArray(vectorDistancias,3);  
  //const char *data = ((String)vectorDistancias[0] + (String)vectorDistancias[1] + (String)vectorDistancias[2]).c_str();
  //Wire.write((uint8_t *)data, strlen(data));
  /*String data = "";
  for (int i=0; i<3; i++){
    int d = abs(vectorDistancias[i]);
    if (d<10) data += "  ";
    else if (d<100) data += " ";
    data += (String) d;
  }
  Serial.println(data);
  for (int i=0; i<9; i++){
    Serial.print(((uint8_t*) data.c_str())[i]);
    Serial.print(",");
  }
  Wire.write((uint8_t*)data.c_str(), 9);*/
  Wire.write(vectorDistancias, 3);
}


/* C implementation QuickSort */
 
// A utility function to swap two elements
void swap(int* a, int* b)
{
  int t = *a;
  *a = *b;
  *b = t;
}
 
/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (int arr[], int low, int high)
{
  int pivot = arr[high];    // pivot
  int i = (low - 1);  // Index of smaller element

  for (int j = low; j <= high- 1; j++)
  {
    // If current element is smaller than or
    // equal to pivot
    if (arr[j] <= pivot)
    {
      i++;    // increment index of smaller element
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[high]);
  return (i + 1);
}
 
/* The main function that implements QuickSort
 * arr[] --> Array to be sorted,
 * low  --> Starting index,
 * high  --> Ending index
 */
void quickSort(int arr[], int low, int high)
{
  if (low < high)
  {
    /* pi is partitioning index, arr[p] is now
       at right place */
    int pi = partition(arr, low, high);

    // Separately sort elements before
    // partition and after partition
    quickSort(arr, low, pi - 1);
    quickSort(arr, pi + 1, high);
  }
}

void printArray(int arr[], int size)
{
    int i;
    for (i=0; i < size; i++)
        Serial.print(arr[i]);
    Serial.print(", ");
}
