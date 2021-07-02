#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string.h>
#include "graph.h"
using namespace std;
//CONSTANTES
const string chord[]={"C","C#","D","Eb","E","F","F#","G","G#","A","Bb","B"}; //Acordes
const float error=0.003; // Margen de error para el histograma
const int kfrg=10;
struct notaux{
	int pitch;
	float tini;
	float dur;
};

struct nota{
	int pitch; //Notas
	float t; //Tiempo en el que se produce un evento
	int e; //Tipo de evento: 0 final de nota 1 inicio
};
struct pianola{
	float t; //Tiempo en donde se produce un evento
	int notas[128]; //Notas presentes en el evento
};
bool operator< (const nota & left, const nota & right){
	bool salida=false;
	if (left.t<right.t) salida=true;
	if (left.t==right.t){
		if (left.e<right.e) salida=true;
	}
	return salida;
}

string nombre(vector <string> mode, string notacion, int a, int e){
	notacion=chord[a]+mode[e];
	return(notacion);
}
	
void escriturafichero (vector <float> tc, float tg, float t, float tb, vector<string> progresion, ofstream &f, ofstream &ft){
		if (f.is_open()){
			for(int i=0; i<(int)tc.size()-1; i++){
				f<<tc[i]<<" "<<progresion[i] <<endl;
			}
			f<<tc[tc.size()-1];
			ft << "Tempo general = "<< tg<<endl;
			ft << "Tempo más repetido en los segmentos = "<< t<<endl;
			ft << "Tempo general con beat = "<< tb<<endl;
		}else{
			cout << "Fichero no encontrado";
		}
}
		
		

void uno(vector <float> repe, float &tempo, int a){
		int cont=0;
		vector <float> tempos;
		for (int i=0; i<(int)repe.size(); i++){/*Calculo de todos los tempos
			sobre los que se tiene que desempatar*/
			if (a==1) tempos.push_back((((float)((int)round(6000/repe[i])))
									   /(float)100));
			else tempos.push_back(repe[i]);
		}
		sort(tempos.begin(), tempos.end());
		mayor*/
		for(int i=(int)tempos.size()-1; i>=0; i--){
			for(int j=i-1; j>=0; j--){
				float t=((float)((int)round(100*(tempos[i]/tempos[j]))))
					/float(100), r;
				int s=t;
				r=t-s;
				if(r==0){
					tempo=tempos[i]; cont=1;
				}else{//Tempos con error permitido
					if(r>=0 && r<=0.125 && cont==0) tempo=tempos[i];
			}
		}
	}
}

		
vector< pair<float,int> >hc(vector <float> a, float e){
		float aux;
		vector< pair<float,int> > h;
		while((int)a.size()!=0){
			int r=0, i=0; aux=a[0];
			while (i<(int)a.size()){/*Buscamos repeticiones que se repiten y
				las eliminamos del vector dur*/
				if (a[i]>aux-e && a[i]<aux+e){
					r++;
					a.erase(a.begin()+i);/*Se eliminan la duración que se
					repite*/
				}else i++;/*Contardor que permite recorrer el vector
				dur*/
			}
			h.push_back(make_pair(aux,r)); /*Se almacena la duración
			analizada y el número de veces que se repite*/
		}
		sort(h.begin(),h.end());
		return(h);
}
		
float tempo2(vector<float> tiempos){
		vector<float> repe;
		float t=0, cont=0;
		vector< pair<float,int> > h;
		h=hc(tiempos, 1);
		for (int o=0; o<(int)h.size(); o++){
			if(h[o].second>cont)cont=h[o].second; /*Duración que más se
			repite*/
		}
		for (int o=0; o<(int)h.size(); o++){
			if(h[o].second==cont) repe.push_back(h[o].first); //Comprobamos
			que no se repita mas de una vez
		}
		if((int)repe.size()==1) t=repe[0]; /*Si solo se repite una vez
		calculamos el tempo directamente*/
		else{ //Formulas de desempate
			uno(repe, t, 0);
			if(t==0) t=repe[0];
		}
		return(t);
}
		
		
float histocal(vector<float> tt){
		int p=0;
		float tempo=0, d=tt[tt.size()-1]-tt[0]; /*"d" duración total del
		segmento*/
		vector <float> dur, repe;
		vector< pair<float,int> > h;
		for (int i=1; i<(int)tt.size(); i++){ /*Calculo de todas las
			duraciones*/
			float daux=tt[i]-tt[i-1];
			if (daux<0) daux=daux*(-1);
			if(daux>0.00009)dur.push_back(daux); /*eliminamos la duraciones
			de armónicas muy pequeñas*/
		}
		h=hc(dur, error);
		for (int i=0; i<(int)h.size(); i++){
			if(h[i].second>p && h[i].first!=0) p=h[i].second; /*Se busca la
			duración que más se repite*/
		}
		for (int i=0; i<(int)h.size(); i++){
			if(p==h[i].second) repe.push_back(h[i].first); //Comprobamos
			que no se repita mas de una vez
		}
		if((int)repe.size()==1){
			tempo=(((float)((int)round(6000/repe[0])))/(float)100); //Si solo
			se repite una vez calculamos el tempo directamente
		}else{
			uno(repe, tempo, 1); //Formulas de desempate
			if(tempo==0) tempo=(((float)((int)round(6000/repe[0])))
				/(float)100);
		}
		return(tempo);
}
		
float histograma(vector<float> &th, vector <float> &tonset, int a){/*Recibe los
		instantes de tiempo en donde se produce un cambio armónico almacenados en
		th del segmento analizado*/
		float aux=0, tempo=0;
		vector <float> tg1(th.begin(), th.end()), tg2, tee;
		copy (th.begin(), th.end(), tg1.begin());
		if(a==0){
			for (int i=0; i<(int)tg1.size(); i++){ /*Nos quedamos solo con
				los instantes de tiempo donde se produce un onset*/
				aux=0;
				for (int j=0; j<(int)tonset.size(); j++){
					if(tg1[i]<tonset[j]+error && tg1[i]>tonset[j]-error){
						aux=1;
					}
				}
				if (aux==1) tg2.push_back(tg1[i]);
			}
		}
		if((int)tg2.size()>3 && a==0) tempo=histocal(tg2); /*Cuando se más
		de tres cambios armónicos en inicio de onset*/
		else tempo=histocal(tg1); /* En caso contrario se usa todo el
		segmento*/
		while(tempo<40 && tempo!=0) tempo=tempo*2;
		while(tempo>300 && tempo!=0) tempo=tempo/2;
		tempo=(((float)((int)round(tempo*100)))/(float)100);
		return(tempo);
}
		
float beat (float t, vector<float> onset, ofstream &fb){
		float frame=60/t, tempo;
		vector<float> on;
		for(float i=onset[0]; i<onset[(int)(onset.size()-1)]; i=i+frame){
			for (int j=0; j<(int)onset.size(); j++){
				if(onset[j]<i+0.05 && onset[j]>i-0.05){
					on.push_back(onset[j]);
					fb<<onset[j]<<endl;	
				}
			}
		}
		tempo=histograma(on, onset, 1);
		return(tempo);
}
		
int calculos (int n[], int r, int a, vector <vector <int> > &Acorde){
		int peso=0, P=0, N=0, M=0, aux[12];
		vector <int> chord(Acorde[a].size());
		for (int i=0; i<12; i++) aux[i]=n[i]; /*Copiamos el segmento a
		analizar, para no modificar el original*/
		for (int i=0; i<(int)Acorde[a].size(); i++){
			chord[i]=Acorde[a][i]+r; /* En cada iteración calculamos la nota
			del acorde con el acorde que probamos sumando r*/
			while (chord[i]>11) chord[i]=chord[i]-12; /*Si se sale de rango
			(0 a 11) restamos 12*/
			if (n[chord[i]]>0){ /* Si la nota del acorde a tratar esta
				presente en el intervalo de tiempo calculamos P y la
				eliminamos de la copia de las notas que están sonando*/
				P=P+n[chord[i]];
				aux[chord[i]]=0;
			}else{ /*En caso de que la nota no se encuentre en el intervalo
				de tiempo incrementamos M*/	
				M++;
			}
		}
		for (int j=0; j<12; j++) N=N+aux[j]; /*Como se ha ido eliminando las
		notas que estaban presentes en el acorde el cálculo
		de N se reduce a la suma del resto de notas*/
		peso= P - ( M + N ); // Aplicamos la formula del peso
		return(peso);
}
		
		
float pesos(int n[], string &notacion, vector <vector <int> > &Acordes, vector<string> &mode){
		int aux=0, no, d;
		float peso=0, paux=0, weights[Acordes.size()];
		for (int i=0; i<12; i++){ /*El índice "i" indica la altura del acorde
			{C, C#, D, (...), B}*/
			if (n[i]>0){ /*Siempre que una nota este presente en el segmento
				a analizar se probara con todos sus acordes bases*/
				for (int j=0; j<Acordes.size(); j++){ /*El índice "k" indica
					el tipo de acorde*/
					weights[j]=calculos(n,i,j, Acordes);
					if(j==0) paux=weights[j]; /*Almacenamos la primera
					iteración para compararla con las demás*/
					if(weights[j]>paux){
						paux=weights[j]; aux=j;
					}
				}
			}
			if (i==0){/*Almacenamos la primera iteración para compararla con
				las demás*/
				peso=paux; d=aux; no=i;
			}
			if (paux>peso ||(paux==peso && aux<d)){
				peso=paux; d=aux; no=i;
			}
		}
		notacion=nombre(mode, notacion, no ,d);
		return(peso);
}
		
void infocamino(vector<int> &caminomaslargo, vector<float> &th, vector<string>&ac, vector<Edge> &edges, vector<string> &prog, vector<pianola> &pfr,string ant){
	for(int i=1; i<(int)caminomaslargo.size(); i++){
		for (int j=0; j<(int)edges.size(); j++) {
			if(caminomaslargo[i-1]==edges[j].first &&
			   caminomaslargo[i]==edges[j].second){
				if (ac.size()==0 && prog[j]!=ant){
					th.push_back(pfr[caminomaslargo[i-1]].t);
					ac.push_back(prog[j]);
				}
				if(ac.size()>0){
					if(prog[j]!=ac[ac.size()-1]){
						th.push_back(pfr[caminomaslargo[i-1]].t);
						ac.push_back(prog[j]);
					}
				}
			}
		}
	}
	th.push_back(pfr[caminomaslargo[caminomaslargo.size()-1]].t);
}
	

float segmentacion(vector<pianola> &pfr, char nombre [], ofstream &ft,vector<string> &progresion, vector <vector <int> > &Acordes,
				   vector<string> &mode, vector <float> &tc, vector <float> &tiempos, vector<float> &tonset, int e, int &co){
		int aux, c, n[12];
		string notacion, ant;
		vector<Edge> edges; //Caminos del grafo
		vector<string> nodenames, prog, ac;
		vector<float> weights, th; /*weights = Pesos, "th" Almacena los
		instantes donde se produce un cambio armónico*/
		//---------------------
		for(int z=0; z<(int)pfr.size(); z++){
			stringstream os; //Nodo en el que se esta trabajando.
			os << z;
			nodenames.push_back(os.str()); 						
			for(int o=0; o<12; o++) n[o]=0; //Inicialización del segmento
			for(int i=z; i<(int)pfr.size()-1; i++){
				for(int j=0; j<128; j++){/*Paso de los pitch a notas,
					perdiendo la información de octava: El resultado es un vector
					de 12 elementos de 0 a 11, cada elemento representa una nota
					C es 0 y B 11 el resto se calcula cromáticamente, el número
					de cada elemento indica el nº de veces que se repite la nota
					en el segmento seleccionado*/
					aux=pfr[i].notas[j];
					if(aux==1){
						c=j%12;
						n[c]=n[c]+1; /*n = notas que están "sonando" en un
						determinado intervalo de tiempo*/
					}
				}
				float peso = pesos(n, notacion, Acordes, mode);/*Se almacena
				el peso en "peso" y el acorde que le corresponde en
				"notacion"*/
				Edge e(z,i+1); // Calculamos el camino correspondiente
				edges.push_back(e);
				weights.push_back(peso);
				prog.push_back(notacion);
			}
		}
		/*longestpath calcula el camino mas largo, devolviendo los nodos
		intermedios que lo componen: está en graph.h*/
		vector<int> caminomaslargo=longestpath(&nodenames[0], (int)
											   nodenames.size(), &edges[0], &weights[0], (int)edges.size(), 1);
		/*Contempla los casos en que el camino más largo va del vértice
		inicial al final*/
		if((int)caminomaslargo.size()==0) caminomaslargo.push_back(0);
		//Se añade el primer y último vértice.
		if((int)caminomaslargo[0]!=0){
			caminomaslargo.insert(caminomaslargo.begin(), 0);
		}
		if(progresion.size()>0) ant=progresion[progresion.size()-1]; /*ant es
		el contador del último acorde que se calculo en el segmento
		anterior*/
		caminomaslargo.push_back(pfr.size()-1);
		infocamino(caminomaslargo, th, ac, edges, prog, pfr, ant);
		/* Almacenamos los tiempos de cambios armónicos, la progresión y el
		tempo de cada segmento*/
		if((int)th.size()>2 || e==1){
			co=1;
			for(int i=1; i<(int)th.size()-1+e; i++){
				tc.push_back(th[i]);
				progresion.push_back(ac[i-1]);
			}
			if (ft.is_open()){
				float tempo=histograma(th, tonset, 0);
				ft << th[0]<< " Tempo = "<< tempo << endl;
				tiempos.push_back(tempo);
			}else{
				cout << "Fichero no encontrado";
			}
		}else co++;
		return(th[th.size()-2+e]);
}
		

vector<pianola>pianol(vector<nota> &notas, float cuant){
			vector<pianola> p(1);
			int aux, cont=0, s[128]; /*cont es el encargado de generar el nuevo
			vector de tiempo*/
			for(int i=0; i<128; i++) s[i]=0;
			for(int i=0; i<(int)notas.size(); i++){
				if(i==0){//Primera iteración
					p[cont].t=notas[i].t;
					aux=notas[i].pitch;
					s[aux]=1;
				}else{
					/*Si se sigue en el mismo instante de tiempo almacenamos las
					notas que esten sonando. cuant = valor de la cuantificación*/
					if(p[cont].t>= notas[i].t - cuant &&
					   p[cont].t <= notas[i].t + cuant){ 
						aux=notas[i].pitch;
						if(notas[i].e==1) s[aux]=1;												
						else s[aux]=0;
					}else{
						for(int j=0; j<128; j++){/*Se copia las notas que están
							sonando en instante anterior
							en el vector p final*/
							if(s[j]==1) p[cont].notas[j]=1;
						}
						/*Primera iteración del
						siguiente instante de tiempo*/
						p.resize(p.size()+1); cont++;
						p[cont].t=notas[i].t;
						aux=notas[i].pitch;
						if(notas[i].e==1) s[aux]=1;												
						else s[aux]=0;
					}
				}
			}
			return(p);
}
			
			

vector<nota>vectortiempo(vector<notaux> &notes){
				vector<nota> aux (notes.size()), notas (notes.size());
				for(int i=0; i<(int)notes.size(); i++){
					notas[i].pitch =notes[i].pitch;
					aux[i].pitch =notes[i].pitch;
					notas[i].t = notes[i].tini;
					aux[i].t = notes[i].tini + notes[i].dur;
					notas[i].e = 1; // nota on
					aux[i].e = 0; // nota off
				}
				for(int i=0; i<(int)aux.size();i++) notas.push_back(aux[i]);
				sort(notas.begin(), notas.end());/*sort devuelve el vector ordenado
				en función del tiempo.*/
				return (notas);
}

				
void procesarLinea(string s, vector<string> &mode, vector< vector <int> >&Acordes){
					istringstream iss(s);
					string token;
					int primero=true;
					vector <int > chord;
					while(getline(iss, token,' ')){
						if (primero)mode.push_back(token);
						else chord.push_back(atoi(token.c_str()));
						primero=false;
					}
					Acordes.push_back(chord);
}
					
					
					
void leerfichero(char v[], vector<notaux> &notes, char a[], vector<string>&mode, vector< vector <int> > &Acordes){
						fstream f, fa;
						f.open(v, ios::in);
						fa.open(a, ios::in);
						//Lectura de la información de entrada
						if(f.is_open()){
							float tini, dur;
							int pitch;
							f>>tini>>dur>>pitch;
							while(!f.eof()){
								notaux n;
								n.tini=tini;
								n.dur=dur;
								n.pitch=pitch;
								notes.push_back(n);
								f>>tini>>dur>>pitch;
							}
							f.close();
						}else cout<< "Error: Fichero de entrada no encontrado"<<endl;
						if (fa.is_open()){
							string s;
							getline(fa,s);
							while (!fa.eof()){
								procesarLinea(s,mode,Acordes);
								getline(fa,s);
							}
							fa.close();
						}else cout<< "Error: Fichero de acordes no encontrado"<<endl;
}
						
	
void principal (char fichero[], char nombre[], char acordes[], float cuant){
							int co=1;
							float cont=0, aux=0, tg;
							vector<notaux> notes;
							vector<nota> notas;
							vector<pianola> p, pfr;
							vector <vector <int> > Acordes;
							//--------------------------
							vector<string> progresion, mode;
							vector <float> tc, tiempos, tonset, duracion;
							leerfichero(fichero, notes, acordes, mode, Acordes);
							for (int i=1; i<(int)notes.size(); i++){
								if(notes[i-1].tini!=notes[i].tini){
									tonset.push_back(notes[i-1].tini);
								}
							}
							notas = vectortiempo(notes);
							p = pianol(notas, cuant); // "p" = pianola
							ofstream f, ft, fb;
							string cad1=string(nombre)+"-Tempos.txt";
							string cad2=string(nombre) + "-Armonia.txt";
							string cad3=string(nombre) + "-beat.txt";
							ft.open(cad1.c_str(), ios::app);
							f.open(cad2.c_str(), ios::app);
							fb.open(cad3.c_str(), ios::app);
							tc.push_back(p[0].t);
							while (aux<p[p.size()-1].t){
								if(p[cont].t+co*kfrg<p[p.size()-1].t &&
								   (p.size()-cont)>100*co){
									for(int i=cont; p[i].t<p[cont].t+kfrg*co; i++){
										pfr.push_back(p[i]);
									}
									aux=segmentacion(pfr, nombre, ft, progresion, Acordes, mode,
													 tc, tiempos, tonset, 0, co);
									if(co==1){
										for(int j=0; j<(int)p.size(); j++){
											if(p[j].t==aux) cont=j;
										}
									}
									pfr.clear(); //Se limpia pfr
								}else{//Caso particular: último trozo del segmento
									for(int i=cont; i<(int)p.size(); i++) pfr.push_back(p[i]);
									aux=segmentacion(pfr, nombre, ft, progresion, Acordes, mode,
													 tc, tiempos, tonset, 1, co);
									aux=p[p.size()-1].t+10; pfr.clear();
								}
							}
							tg=histograma(tc, tonset, 1);
							float t=tempo2(tiempos);
							float tb = beat(tg, tonset, fb);
							escriturafichero(tc, tg, t, tb, progresion, f, ft);
							ft.close();f.close(); fb.close();
}
							
int main(int argc, char *argv[]){
	if (argc>5 || argc<4)
		cout << "Sintaxis: ./armonias entrada.txt prefijosalida.txt";
		cout << "tablaacordes.txt valorcuantizacion" <<endl;
	else{
		if (argc==5){
			principal (argv[1], argv[2], argv[3], atof(argv[4]));
		}
		if (argc==4){
			principal (argv[1], argv[2], argv[3], 0);
		}
	}
	return 0;
}
