/*#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
using namespace std;

int main(int argc, char* argv[]) {
double lat1=-91.0,lat2=91.0,long1=-181.0,long2=181.0;
do {
cout << ''Enter first latitude and longitude in decimal format:'';
cin >> lat1 >> long1;
} while (lat1 < -90.0 || lat1 > 90.0 || long1 > 180.0 || long1 < -180.0);
do {
cout << ''\nEnter second latitude and longitude in decimal format:'';
cin >> lat2 >> long2;
cout << endl;
} while (lat2 < -90.0 || lat2 > 90.0 || long2 > 180.0 || long2 < -180.0);


// Below is the main code
double PI = M_PI;  // 4.0*atan(1.0);

//main code inside the class
double dlat1=lat1*(PI/180);

double dlong1=long1*(PI/180);
double dlat2=lat2*(PI/180);
double dlong2=long2*(PI/180);

double dLong=dlong1-dlong2;
double dLat=dlat1-dlat2;

double aHarv= pow(sin(dLat/2.0),2.0)+cos(dlat1)*cos(dlat2)*pow(sin(dLong/2),2);
double cHarv=2*atan2(sqrt(aHarv),sqrt(1.0-aHarv));
//earth's radius from wikipedia varies between 6,356.750 km - 6,378.135 km (~3,949.901 - 3,963.189 miles)
//The IUGG value for the equatorial radius of the Earth is 6378.137 km (3963.19 mile)
const double earth=3963.19;//I am doing miles, just change this to radius in kilometers to get distances in km
double distance=earth*cHarv;

cout << ''...Distance is '' << distance << '' miles.\n'';

return 0;
}
// A couple of values from Pierce:
// 34.184803 -118.574251 Pool
// 34.184695 -118.586594 Equestrian
*/

/*/ CS546 Sample Intel AVX Register Usage
//

#include <iostream>
#include <iomanip>
#include <float.h>
#include ''windows.h''
using namespace std;

ULONGLONG getTime64(LPFILETIME a);

#define CS546_ARRAY_SIZE 9600000
#define CS546_FLOAT_BITS 32
#define CS546_REGISTER_BITS 256
#define CS546_STRIDE_SIZE CS546_REGISTER_BITS / CS546_FLOAT_BITS

int main(int argc, char* argv[])
{
unsigned int i, j;
alignas(32) static float a[CS546_ARRAY_SIZE] = { 0 };
alignas(32) static float b[CS546_ARRAY_SIZE] = { 0 };
alignas(32) static float c[CS546_ARRAY_SIZE] = { 0 };
alignas(32) static float x[CS546_ARRAY_SIZE];
alignas(32) static float results[CS546_ARRAY_SIZE] = { 0 };
float ax2;
short cWord;
short maskOffPC = 0x0c3f; // set RC and PC differently
short turnOnRC = 0x0c00l; // turn on RC bits

int mxcWord = 0x00007f80;  // default + round towards zero

// CPU timing variables
FILETIME creationTime, exitTime, kernelTime, userTime;
LPFILETIME creation = &creationTime, exit = &exitTime, kernel = &kernelTime, user = &userTime;
ULONGLONG c1, c2, e1, e2, k1, k2, u1, u2;
HANDLE myProcess = GetCurrentProcess();
BOOL gotTime1, gotTime2;
DWORD failReason;
SYSTEMTIME loct;

// check the sizes
j = CS546_ARRAY_SIZE % CS546_STRIDE_SIZE;
if (j) {
cout << ''Array size '' << CS546_ARRAY_SIZE <<
'' is not an even multiple of the AVX register size - ending.\n'';
return -1;
}

// Fill the array with random values.
for (i = 0; i < CS546_ARRAY_SIZE; i++) {
x[i] = static_cast<float>(i);
srand(i);
a[i] = static_cast<float>(i * 386);
b[i] = static_cast<float>(i * 1511);
c[i] = static_cast<float>(i * 546);
}

std::cout << ''====================\n'';

// CPU timing initial measurements
GetLocalTime(&loct);
cout << ''The starting local time is:'' << loct.wHour << ':' << loct.wMinute << ':' << loct.wSecond << '.' << loct.wMilliseconds << endl;
double fStartTime = loct.wHour * 3600 + loct.wMinute * 60 + loct.wSecond + (loct.wMilliseconds / 1000.0);

gotTime1 = GetProcessTimes(myProcess, creation, exit, kernel, user);
if (!gotTime1) {
failReason = GetLastError();
cout << ''GetProcessTimes failed, Failure reason:'' << failReason << endl;
return 0;
}
c1 = getTime64(creation);
e1 = getTime64(exit);
k1 = getTime64(kernel);
u1 = getTime64(user);

// equivalent C++ code for the loop we are running:
/*      for (int i = 0; i < CS546_ARRAY_SIZE; i++) {
results[i] = a[1] * pow(x[i], 2) + b[i] * x[i] + c[i];
}
*/
/*
// The loop to be timed
__asm {
; Initialize AVX registers
ldmxcsr mxcWord

; Set up array address and count.
mov     ecx, CS546_ARRAY_SIZE
mov             edi, 0

; Loop through array
myLoop:
vmovaps ymm0, x[edi]
vmovaps ymm1, ymm0
vmulps  ymm0, ymm1, ymm0
vmovaps ymm2, a[edi]
vmulps  ymm0, ymm2, ymm0
vmovaps ymm3, b[edi]
vmulps  ymm1, ymm3, ymm1
vmovaps ymm4, c[edi]
vaddps  ymm1, ymm4, ymm1
vaddps  ymm0, ymm1, ymm0
vmovaps results[edi], ymm0
add             edi, 4 * CS546_STRIDE_SIZE
sub             ecx, CS546_STRIDE_SIZE
inc             ecx
loop    myLoop
}


// Get CPU ending timings
gotTime2 = GetProcessTimes(myProcess, creation, exit, kernel, user);
if (!gotTime2) {
failReason = GetLastError();
cout << ''GetProcessTimes failed, Failure reason : '' << failReason << endl;
return 0;
}
u2 = getTime64(user);
double workUserTime = (u2 - u1) * 100.0 / 1000000000.0;
cout << ''Process user time : '' << workUserTime << '' ('' << u2 - u1 << ')' << endl;

GetLocalTime(&loct);
cout << ''The   ending local time is : '' << loct.wHour << ':' << loct.wMinute << ':' << loct.wSecond << '.' << loct.wMilliseconds << endl;
double fEndTime = loct.wHour * 3600 + loct.wMinute * 60 + loct.wSecond + (loct.wMilliseconds / 1000.0);
double workTime = fEndTime - fStartTime;
cout << ''Elapsed clock time : '' << workTime << endl;
cout << ''CPU busy percentage : '' << (workUserTime / workTime) * 100 << endl;

/*      Print the array for debugging
for (i = 0, j = 1; i < CS546_ARRAY_SIZE; i++, j++) {
std::cout << setw(3) << i << '|' << setw(11) << fixed << setprecision(2) << results[i] << '|';
if (j > 3) {
j = 0;
std::cout << endl;
}
}

std::cout << ''\nDone.'' << endl;

return 0;
}

// convert a FILETIME structure to a 64-bit integer
ULONGLONG getTime64(LPFILETIME a) {
ULARGE_INTEGER work;
work.HighPart = a->dwHighDateTime;
work.LowPart = a->dwLowDateTime;
return work.QuadPart;
}


// Loop with x87 floating point instructions:
//              ; Initialize x87
FINIT
FSTCW   cWord
MOV     AX, cWord
AND     AX, maskOffPC
or  AX, turnOnRC
MOV     cWord, AX
FLDCW   cWord
FLDZ;
; Set up array address and count.
LEA     EBX, x
LEA EAX, results
MOV     ECX, CS546_ARRAY_SIZE


; Set up array address and count.
mov     ecx, CS546_ARRAY_SIZE
mov             edi,0

; Loop through array
myLoop:
FLD     x[edi]
FLD             ST(0)
FMUL    ST(0), ST(1)
FMUL    a[edi]
FSTP    ax2
FMUL    b[edi]
FADD    c[edi]
FADD    ax2
FSTP    results[edi]
ADD             edi, 4
LOOP    myLoop
*/
// AVX version
/*
; Initialize AVX registers
ldmxcsr mxcWord

; Set up array address and count.
mov     ecx, CS546_ARRAY_SIZE
mov             edi,0

; Loop through array
myLoop:
vmovups ymm0, x[edi]
vmovups ymm1,ymm0
vmulps  ymm0,ymm1,ymm0
vmovups ymm2,a[edi]
vmulps  ymm0,ymm2,ymm0
vmovups ymm3,b[edi]
vmulps  ymm1,ymm3,ymm1
vmovups ymm4,c[edi]
vaddps  ymm1,ymm4,ymm1
vaddps  ymm0,ymm1,ymm0
vmovups results[edi],ymm0
add             edi, 4* CS546_STRIDE_SIZE
sub             ecx,CS546_STRIDE_SIZE
inc             ecx
loop    myLoop
*/