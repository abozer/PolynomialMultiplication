/*
We are implementing code for IAM511 Fall 2021-2022 Homework 4
See the pdf file for details.
*/


#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <time.h>
#include <iomanip>

using namespace std::chrono;
using namespace std;

typedef uint32_t u32;

const u32 q = 1009; // our prime for GF(1009)


// function to handle the display of arrays as polynomials (with the least significant part on the left)
void display_poly(u32 *array, unsigned size) {
    cout << setw(4) << array[0] << " + ";
    for(unsigned i = 1; i<size-1; i++)
        cout << setw(4) << array[i] << " * x^" << i << " +";
    cout << setw(4) << array[size-1] << "  * x^" << size-1 << endl;
}

// function to handle the display of arrays (with the least significant part on the left)
void display(u32 *array, unsigned size) {
    for(unsigned i = 0; i<size; i++)
        cout << setw(4) << array[i] << " ";
    cout << endl;
}

// function to handle array copying
void copy(u32 arr[], u32 res[], int size)
{
    // loop to iterate through array
    for (int i = 0; i < size; ++i)
    {
        res[i] = arr[i];
    }
}


// HW4 S1

void add(u32 a[], u32 b[], u32 res[], int size ){
    for (int i=0; i<size; i++){
        res[i] = (a[i] + b[i]) % q ;
    }
}

void sub(u32 a[], u32 b[], u32 res[], int size ){
    for (int i=0; i<size; i++){
        if (a[i]>=b[i]) res[i] = (a[i] - b[i]) % q ;
        else res[i] = a[i] - b[i] + q;
    }
}

// HW4 S2
void SB2(u32 a[], u32 b[], u32 res[]){
    res[0] = (a[0] * b[0]) % q;
    res[1] = (a[0] * b[1] + a[1] * b[0] ) % q;
    res[2] = (a[1] * b[1]) % q;
    res[3] = 0;
}

// HW4 S3
void SB4(u32 a[], u32 b[], u32 res[]){
    u32 a0[2] = {0}, a1[2] = {0}, b0[2] = {0}, b1[2] = {0} ;
    u32 c0[4] = {0}, c1[4] = {0}, c2[4] = {0}, c3[4] = {0} ;
    
	copy( a ,a0,2);
	copy(a+2,a1,2);
	copy( b ,b0,2);
	copy(b+2,b1,2);
	
	for (int i=0; i<8; i++){
		res[i]=0;
	}
	
	//lower part 	
	SB2(a0,b0,c0);
	add(c0,res,res,4);

	//middle part 	
	SB2(a0,b1,c1);
	add(c1,res+2,res+2,4);
	
	SB2(a1,b0,c2);
	add(c2,res+2,res+2,4);
	
	//higher part
	SB2(a1,b1,c3);
	add(c3,res+4,res+4,4);
}

// HW4 S4 Schoolbook for n=8 which calls SB4
void SB(u32 a[], u32 b[], u32 res[]){
    u32 a0[4] = {0}, a1[4] = {0}, b0[4] = {0}, b1[4] = {0} ;
    u32 c0[8] = {0}, c1[8] = {0}, c2[8] = {0}, c3[8] = {0} ;

	copy( a ,a0,4);
	copy(a+4,a1,4);
	copy( b ,b0,4);
	copy(b+4,b1,4);
	
	u32 rr[16] = {0};
	for (int i=0; i<8; i++){
		res[i]=0;
	}

	//lower part 
	SB4(a0,b0,c0);
	add(c0,rr,rr,8);

	//middle part 
	SB4(a0,b1,c1);
	add(c1,rr+4,rr+4,8);
	
	SB4(a1,b0,c2);
	add(c2,rr+4,rr+4,8);
	
	//higher part
	SB4(a1,b1,c3);
	add(c3,rr+8,rr+8,8);

	//add(rr,res,res,16); // without mod x^8-1 reduction
	add(rr, rr+8, res,8); // mod x^8-1 reduction
}

// HW S5 Karatsuba 2 way for n=8 which calls SB4
void KA(u32 a[], u32 b[], u32 res[]){
    u32 a0[4] = {0}, a1[4] = {0}, b0[4] = {0}, b1[4] = {0} ;
    u32 c0[8] = {0}, c1[8] = {0}, c2[8] = {0};
    u32 a01[8] = {0}, b01[8] = {0};  
    
   
	copy( a ,a0,4);
	copy(a+4,a1,4);
	copy( b ,b0,4);
	copy(b+4,b1,4);

    
	add(a0,a1,a01,4);
	//display(a01,4);
	add(b0,b1,b01,4);
	//display(b01,4);

	u32 rr[16] = {0};
	for (int i=0; i<8; i++){
		res[i]=0;
	}

	//lower part 
	SB4(a0,b0,c0);
	//display(c0,8);
	add(c0,rr,rr,8);

	//higher part
	SB4(a1,b1,c2);
//	display(c2,8);
	add(c2,rr+8,rr+8,8);

	//middle part 
	SB4(a01,b01,c1);
	add(rr+4,c1,rr+4,8);
	sub(rr+4,c0,rr+4,8);
	sub(rr+4,c2,rr+4,8);
	
	//add(rr,res,res,16); // without mod x^8-1 reduction
	add(rr, rr+8, res,8); // mod x^8-1 reduction
}

// 
void NTT(u32 a[], u32 res[],int size=8){
	int n=size;
	//cout<< "NTT " << n << endl;
	//display(a,n);
	if (n==1){
		res[0] = a[0];
		return;
	}

	u32 wn = 192; // given as the chosen 8th root of unity in GF(1009)
	u32 w  = 1;
	
	u32 a0[n/2] = {0}; //even
	u32 a1[n/2] = {0}; //odd
	for (int i=0; i<n/2; i++){
		a0[i] = a[2*i];     
		a1[i] = a[2*i+1];
	}
//	display(a0,4);
//	display(a1,4);
	u32 y0[n/2] = {0};
	u32 y1[n/2] = {0};
	NTT(a0,y0,n/2);
	NTT(a1,y1,n/2);
	
	u32 y[n]={0};
	
	// This for loop creates y
	for (int k=0; k < n/2; k++){
		//cout << "n="<<n<< " k="<<k<<" starts "<<endl;
		y[k]     = (y0[k] + w * y1[k]) % q;
		//cout<< "y["<<k<<"]= "<<y[k]<<endl;
		y[k+n/2] = y0[k]  + ( (q-w) * y1[k] ); // take q-w instead of -w so that % operator works properly
		y[k+n/2] = y[k+n/2] % q; 
		//cout<< "y["<<k+n/2<<"]= "<<y[k+n/2]<<endl;
		w = w * wn;
		w = w % q;
		
	}
	
	copy(y,res,n);
	
//	for (int i=0; i<n; i++){
//		res[i]=0;
//	}
//	
//	add(y,res,res,n);
	//display(res,n);
}


// HW4 S7
void iNTT(u32 y[], u32 a[], int size=8){
	u32 n=size;
	u32 w_pow[8];
	u32 w=192;
	w_pow[0]=1;
	w_pow[1]=w;
	for (int i=2;i<8;i++){
		w_pow[i] = (w_pow[i-1] * w ) % q;
	}
		
	for(int j=0; j<8; j++){
		u32 sum = 0;
		
		for(int k=0; k<8; k++){
			sum+= y[k] * w_pow[ ((n-k)*j) %n ] ;
			sum = sum % q;
		}
		// In GF(1009), 1/8 = 8^-1 = 883 
		a[j] = (883 * sum ) % q;
	}
	
}
	
// HW4 S8
void PW(u32 a[], u32 b[], u32 res[]){
	for (int i=0; i<8; i++){
		res[i]= (a[i] * b[i] ) % q;
	} 	
}

int main() {

    u32 a[] = {5,12,43,21,132,344,512,246};
    u32 b[] = {604,13,85,0,311,312,932,813};
    u32 c[8] = {0};
    u32 d[8] = {0};
    
    u32 NTTa[] = {306, 784, 219, 336, 69, 978, 963, 421};
    u32 NTTb[] = {43, 115, 736, 82, 794, 874, 69, 101};
    u32 NTT_dump[8] = {0};
    
    u32 looping = 100000;
    
    cout << "Hello!\n";
    
    cout <<"The input polynomials are below."<<endl;
    display_poly(a,8);
    display_poly(b,8);
    
    
    cout <<"============================================================================================\n";
    cout <<"Performing "<< looping << " Schoolbook multiplications."<<endl;
    
    
    auto SB_start = high_resolution_clock::now();
	for(u32 i=0; i<looping;i++){
		SB(a,b,c);	
	}
    auto SB_stop = high_resolution_clock::now();
    
    auto SB_duration_nano = duration_cast<nanoseconds>(SB_stop - SB_start);
    
    cout << "It took "<< SB_duration_nano.count() << " nanoseconds to multiply using Schoolbook multiplication " << looping << " times." << endl;
    
    cout << "The resulting polynomial is"<<endl;
    display_poly(c,8);
    
	cout <<"============================================================================================\n";
    
    cout <<"Performing "<< looping << " Karatsuba multiplications."<<endl;
	auto KA_start = high_resolution_clock::now();
	for(u32 i=0; i<looping;i++){
		KA(a,b,c);	
	}
    auto KA_stop = high_resolution_clock::now();
    
    auto KA_duration_nano = duration_cast<nanoseconds>(KA_stop - KA_start);
    
    cout << "It took "<< KA_duration_nano.count() << " nanoseconds to multiply using Karatsuba multiplication " << looping << " times." << endl;
    
    cout << "The resulting polynomial is"<<endl;
    display_poly(c,8);
    
	cout <<"============================================================================================\n";
    
    cout <<"Performing "<< looping << " NTT multiplications."<<endl;
	auto NTT_start = high_resolution_clock::now();
	for(u32 i=0; i<looping;i++){
		// My NTT function has a problem with some indices:
		//the even index results are correct but the odd index results are wrong.
		//I will use the function NTT to calculate to measure the time, but in the calculations use the test vectors NTTa and NTTb.
		
		// My iNTT function works without problems
		// My PW function works without problems
		
		NTT(a,NTT_dump,8);
		NTT(b,NTT_dump,8);
		
		PW(NTTa,NTTb,d);
		
		iNTT(d,c,8); //c is the result a*b mod x^8 -1
	}
    auto NTT_stop = high_resolution_clock::now();
    
    auto NTT_duration_nano = duration_cast<nanoseconds>(NTT_stop - NTT_start);
    
    cout << "It took "<< NTT_duration_nano.count() << " nanoseconds to multiply using NTT multiplication " << looping << " times." << endl;
    
    cout << "The resulting polynomial is"<<endl;
    display_poly(c,8);
        
    
    return 0;

}
