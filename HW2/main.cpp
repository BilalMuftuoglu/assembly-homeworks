#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "image_processing.cpp"

using namespace std;

void Dilation(int n, int filter_size, short* resimadres_org);
void Erosion(int n, int filter_size, short* resimadres_org);

int main(void) {
	int M, N, Q, i, j, filter_size;
	bool type;
	int efile;
	char org_resim[100], dil_resim[] = "dilated.pgm", ero_resim[] = "eroded.pgm";
	do {
		printf("Orijinal resmin yolunu (path) giriniz:\n-> ");
		scanf("%s", &org_resim);
		system("CLS");
		efile = readImageHeader(org_resim, N, M, Q, type);
	} while (efile > 1);
	int** resim_org = resimOku(org_resim);

	printf("Orjinal Resim Yolu: \t\t\t%s\n", org_resim);

	short *resimdizi_org = (short*) malloc(N*M * sizeof(short));

	for (i = 0; i < N; i++)
		for (j = 0; j < M; j++)
			resimdizi_org[i*N + j] = (short)resim_org[i][j];

	int menu;
	printf("Yapmak istediginiz islemi giriniz...\n");
	printf("1-) Dilation\n");
	printf("2-) Erosion\n");
	printf("3-) Cikis\n> ");
	scanf("%d", &menu);
	printf("Filtre boyutunu giriniz: ");
	scanf("%d", &filter_size);


	switch (menu){
		case 1:
			Dilation(N*M, filter_size, resimdizi_org);
			resimYaz(dil_resim, resimdizi_org, N, M, Q);
			break;
		case 2:
			Erosion(N*M, filter_size, resimdizi_org);
			resimYaz(ero_resim, resimdizi_org, N, M, Q);
			break;
		case 3:
			system("EXIT");
			break;
		default:
			system("EXIT");
			break;
	}

	system("PAUSE");
	return 0;
}

void Dilation(int n, int filter_size, short* resim_org) {
	__asm {
			//resmin kare oldugu bilindigi icin satir veya sutun sayisi n'nin 
			//karekoku hesaplanarak bulunur. islem sonunda ecx = satir sayisi
			mov ebx, n
			mov ecx, 0
			mov edx, 0
		iter:
			inc ecx
			mov eax, ecx
			mul ecx
			cmp eax, ebx
			jge done
			jmp iter
		done : ;//ecx = sqrt(n) = satir sayisi

			mov esi, 0; // esi = i

		start2:
			cmp esi, n; // i == n
			je exit2; // dongu sona erer
			mov edx, filter_size
			shr edx, 1;// filter_size/2
			mov ebx, edx
			neg ebx;// ebx = j = -filter_size/2
			mov eax, 0; // eax = max

		start3:
			mov edx, filter_size;
			shr edx, 1;// filter_size/2
			cmp ebx, edx; // j == filter_size/2
			jg exit3; // dongu sona erer
			mov edi, edx
			neg edi; // edi = k = -filter_size/2

		start4 :
			mov edx, filter_size
			shr edx, 1;// filter_size/2
			cmp edi, edx; // k == filter_size/2
			jg exit4; // dongu sona erer

			push ebx;// j degerini korumak icin push islemi
			push eax;// max degerini korumak icin push islemi
			mov eax, ecx;// eax = satir sayisi
			xor edx, edx
			mul ebx;// eax = j * satir sayisi
			add eax, esi;// eax = i + j * satir sayisi
			add eax, edi; // eax = i + j * satir sayisi + k

			cmp eax, 0;// Eger eax 0'dan kucukse dizi disina cikildigindan devam edilmez
			jl exit5
			cmp eax, n; // Eger eax n'ye buyuk esitse dizi disina cikildigindan devam edilmez
			jge exit5

			pop ebx;// stack'e en son atilan max degeri ebx'e cekilir
			//ama stack'de de durmasi istenildiginden tekrar push yapilir
			push ebx
			xchg eax, ebx;// eax = max , ebx = i + j * satir sayisi + k
			push esi;// i degerini korumak icin push islemi
			mov esi, resim_org
			shl ebx, 1
			cmp word ptr[esi + ebx], ax;// resim_org[i + j * satir sayisi + k] == max
			jle exit6;//eger dizide kontrol edilen deger max'dan kucuk esitse devam edilmez
			mov ax, word ptr[esi + ebx];// yeni max degeri ax'e alinir
		exit6:
			shr ebx, 1
			pop esi
			pop edx;// cop deger
			pop ebx
			inc edi;// k++
			jmp start4
		exit5 :
			pop eax
			pop ebx
			inc edi; // k++
			jmp start4; // basa don
		exit4:
			inc ebx; // j++
			jmp start3; // basa don
		exit3:
			push ax // max elemani bulma islemi bitti, max deger stack'e atilir
			inc esi; // i++
			jmp start2; // basa don
		exit2:
			mov ecx,n // tum donguler sona erdi
			mov edi,n // stackten max degerler teker teker alinip diziye sondan basa dogru eklenir
			mov esi,resim_org
		son:
			dec edi
			pop ax
			shl edi,1
			mov word ptr [esi+edi],ax
			shr edi,1
			loop son
	};
	printf("\nDilation islemi sonucunda resim \"dilated.pgm\" ismiyle olusturuldu...\n");
}


void Erosion(int n, int filter_size, short* resim_org) {
	__asm {
			//resmin kare oldugu bilindigi icin satir veya sutun sayisi n'nin 
			//karekoku hesaplanarak bulunur. islem sonunda ecx = satir sayisi
			mov ebx, n
			mov ecx, 0
			mov edx, 0
		iter:
			inc ecx
			mov eax, ecx
			mul ecx
			cmp eax, ebx
			jge done
			jmp iter
			done : ;//ecx = sqrt(n) = satir sayisi

			mov esi, 0; // esi = i

		start2:
			cmp esi, n; // i == n
			je exit2; // dongu sona erer
			mov edx, filter_size
			shr edx, 1;// filter_size/2
			mov ebx, edx
			neg ebx;// ebx = j = -filter_size/2
			mov eax, 255; // eax = min

		start3:
			mov edx, filter_size;
			shr edx, 1;// filter_size/2
			cmp ebx, edx; // j == filter_size/2
			jg exit3; // dongu sona erer
			mov edi, edx
			neg edi; // edi = k = -filter_size/2

		start4:
			mov edx, filter_size
			shr edx, 1;// filter_size/2
			cmp edi, edx; // k == filter_size/2
			jg exit4; // dongu sona erer

			push ebx;// j degerini korumak icin push islemi
			push eax;// min degerini korumak icin push islemi
			mov eax, ecx;// eax = satir sayisi
			xor edx, edx
			mul ebx;// eax = j * satir sayisi
			add eax, esi;// eax = i + j * satir sayisi
			add eax, edi; // eax = i + j * satir sayisi + k

			cmp eax, 0;// Eger eax 0'dan kucukse dizi disina cikildigindan devam edilmez
			jl exit5
			cmp eax, n; // Eger eax n'ye buyuk esitse dizi disina cikildigindan devam edilmez
			jge exit5

			pop ebx;// stack'e en son atilan min degeri ebx'e cekilir ama stack'de de durmasi istenildiginden tekrar push yapilir
			push ebx
			xchg eax, ebx;// eax = min , ebx = i + j * satir sayisi + k
			push esi;// i degerini korumak icin push islemi
			mov esi, resim_org
			shl ebx, 1
			cmp word ptr[esi + ebx], ax;// resim_org[i + j * satir sayisi + k] == min
			jge exit6;//eger dizide kontrol edilen deger min'den buyuk esitse devam edilmez
			mov ax, word ptr[esi + ebx];// yeni min degeri ax'e alinir
		exit6:
			shr ebx, 1
			pop esi
			pop edx// cop deger
			pop ebx
			inc edi;// k++
			jmp start4
		exit5 :
			pop eax
			pop ebx
			inc edi; // k++
			jmp start4; // basa don
		exit4:
			inc ebx; // j++
			jmp start3; // basa don
		exit3:
			push ax // max elemani bulma islemi bitti, max deger stack'e atilir
			inc esi; // i++
			jmp start2; // basa don
		exit2:
			mov ecx, n // tum donguler sona erdi
			mov edi, n // stackten max degerler teker teker alinip diziye sondan basa dogru eklenir
			mov esi, resim_org
		son :
			dec edi
			pop ax
			shl edi, 1
			mov word ptr[esi + edi], ax
			shr edi, 1
			loop son
	};
	printf("\nErosion islemi sonucunda resim \"eroded.pgm\" ismiyle olusturuldu...\n");
}