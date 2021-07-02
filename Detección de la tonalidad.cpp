#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
using namespace std;
const string chord[]=
{"C","C#","D","Eb","E","F","F#","G","G#","A","Bb","B"};
/*Ordena el histograma de mayor a menor*/
bool operator< (const pair<string, float> & left, const pair<string,
				float> & right){
	bool salida=false;
	if (left.second<right.second) salida=true;
	return salida;
}

void cambio(string a, int &n){
	for (int i=0; i<12; i++){
		if (a[0]==chord[i][0] && chord[i][1]!='#' && chord[i][1]!='b')
			n=i;
		if (a[0]==chord[i][0] && a[1]==chord[i][1]) n=i;
	}
}
	

string tonalidad(vector< pair<string, float> > &p){
		vector< pair<string, float> > h;
		vector< pair<string, float> > cont;
		float c=0;
		string result;
		for (int i=1; i<p.size(); i++){ /* Se recorre todos los cambios
			armónicos*/
			int n1, n2;
			cambio(p[i].first, n1);
			cambio(p[i-1].first, n2);
			int d1=n2-n1;
			if((d1==7 || d1==-5) && (p[i].first[1]=='M' ||
									 p[i].first[2]=='M' || p[i].first[1]=='m' || p[i].first[2]=='m')){
				float d = (p[i].second + p[i-1].second);
				cont.push_back(make_pair(p[i].first, d)); /*Se almacena el
				acorde base y el tempo que se repite*/
			}
		}
		if(cont.size()==0){
			for (int i=1; i<p.size(); i++){
				int n1, n2;
				cambio(p[i].first, n1);
				cambio(p[i-1].first, n2);
				int d1=n2-n1;
				if(d1==7 || d1==-5){
					string a = p[i].first;
					if(p[i].first[1]=='7' || p[i].first[1]=='A'
					   || p[i].first[1]=='D' || p[i].first[1]=='4'
					   || p[i].first[1]=='2'){
						string s; s = p[i].first[0];
						if(p[i-1].first[1]=='m' ||
						   p[i-1].first[2]=='m' || (p[i-1].first[1]=='D'
													&& p[i-1].first[2]=='i') || (p[i-1].first[2]=='D'
																				 && p[i-1].first[3]=='i')) a = s + "m";
						else a = s + "M";
					}
					if(p[i].first[2]=='7' || p[i].first[2]=='A' ||
					   (p[i].first[2]=='D' && p[i].first[3]=='i') ||
					   p[i].first[2]=='4' || p[i].first[2]=='2'){
						string s , s1;
						s = p[i].first[0], s1 = p[i].first[1];
						if(p[i-1].first[1]=='m' ||
						   p[i-1].first[2]=='m' || (p[i-1].first[1]=='D'
													&& p[i-1].first[2]=='i') || (p[i-1].first[2]=='D'
																				 && p[i-1].first[3]=='i')) a = s + s1 + "m";
						else a = s + s1 + "M";
					}
					float d = (p[i].second + p[i-1].second);
					cont.push_back(make_pair(a, d));
				}
			}
		}
		sort(cont.begin(), cont.end());
		while((int)cont.size()!=0){
			int i=0;
			float r=0;
			string aux12=cont[0].first;
			while (i<(int)cont.size()){
				if (cont[i].first==aux12){
					r=r+cont[i].second;
					cont.erase(cont.begin()+i);
				}else{
					if (cont[i].first[0]==aux12[0] &&
						cont[i].first[1]!=aux12[1]){
						r=r-cont[i].second; cont.erase(cont.begin()+i);
					}else i++;
				}
			}
			h.push_back(make_pair(aux12,r));
		}
		for (int o=0; o<(int)h.size(); o++){
			if(h[o].second>c){
				c=h[o].second;
				result=h[o].first;
			}
		}
		return (result);
}

		

void leerfichero(char v[], vector< pair<string, float> > &p){
			fstream f;
			vector< pair<string, float> > aux;
			f.open(v, ios::in);
			if(f.is_open()){
				string name;
				float to;
				f>>to;
				while(!f.eof()){
					f>>name;
					aux.push_back(make_pair(name, to));
					f>>to;
				}
				aux.push_back(make_pair(" ", to));
				f.close();
			}else{
				cout<< "Error: Fichero no encontrado"<<endl;
			}
			for(int i=1; i<aux.size(); i++){
				float dur= aux[i].second-aux[i-1].second;
				p.push_back(make_pair(aux[i-1].first, dur));
			}
}
			
int main(int argc, char *argv[]){
	vector< pair<string, float> > p;
	string t;
	if (argc!=2) cout << "Sintaxis: ./tonalidad entrada.txt" <<
		endl;
	else{
		leerfichero(argv[1], p);
		t=tonalidad(p);
		cout<<t<<endl;
	}
	return 0;
}
					
