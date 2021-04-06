#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
	int i, j; //koordinate el u matrici
}cord;
typedef struct {
	cord m;
	int edg; //indeks od koga pocinju susedi u indices
	int check;
}eledg;
typedef struct {
	int neig;
	char udlr;
	int check;
}elind;
typedef struct {
	int pos, ori;
	int s[2];
}state;
int**mat_input_form(int *m,int*n,cord *s,cord *t) { //formira matricu jednaku matrici char datoj u datoteci
	char*str = malloc(30);
	if (!str) exit(-1);
	printf("Unesite ime datoteke:\n");
	scanf("%s", str);
	FILE*dat = fopen(str, "r");
	char c = fgetc(dat);
	int i = 0,j=0,br=0;
	int **mat;
	mat = malloc(30 * sizeof(int*));
	mat[0] = malloc(30 * sizeof(int));
	while (c != EOF) {
		while ((c != '\n')&&c!=EOF) {
			if (c == '–') mat[i][j++] = -1;
			else {
				if (c == 'S') {
					s->i = i;
					s->j = j;
				}
				if (c == 'T') {
					t->i = i;
					t->j = j;
				}
				mat[i][j++] = br++;
			}
			c = fgetc(dat);
		}
		i++;
		if (c == '\n') {
			mat[i] = malloc(30 * sizeof(int));
			j = 0;
			c = fgetc(dat);
		}
		
	}
	*m = i;
	*n = j;
	fclose(dat);
	return mat;
}
int graphform(int**mat,int m,int n, eledg *edges, elind *indices) {
	int k=0, l=0;

	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if (mat[i][j] != -1) {
				edges[k].m.i = i; //ubacuje cvor sa koordinatama i j i indeksom prvog elementa u ind
				edges[k].m.j = j;
				edges[k].check = 0;
				edges[k++].edg = l;
				//ubacuje susede u ind id cvora i koji je sused
				if (j>0&&mat[i][j - 1] != -1) { 
					indices[l].neig = mat[i][j - 1];
					indices[l].check = 0;
					indices[l++].udlr = 'l';
				}
				if (j<n-1&&mat[i][j +1] != -1) {
					indices[l].neig = mat[i][j +1];
					indices[l].check = 0;
					indices[l++].udlr = 'r';
				}
				if (i>0&&mat[i-1][j] != -1) {
					indices[l].neig = mat[i-1][j];
					indices[l].check = 0;
					indices[l++].udlr = 'u';
				}
				if (i<m-1&&mat[i+1][j] != -1) {
					indices[l].neig = mat[i+1][j];
					indices[l].check = 0;
					indices[l++].udlr = 'd';
				}

				
			}
		}
	}
	edges[k].edg = l; //poslednji cvor je nepostojeci vec samo pok na kraj niza ind
	return k; //poslednji el u nizu cvorova (koji pokazuje na kraj)
}
int ins_edge(int x, int y, eledg *edges, elind *indices,int i,int j,int k ,int a,char c) { //ubacivanje grana 
	if (x == i && y == j) { //ako je cvor odg sused
		int o;
		while (edges[a + 1].edg == -1) a++;
		for (o = edges[k].edg; o > edges[a + 1].edg; o--) { //pomeramo ind od kraja do indeksa prvog sledeceg cvora koji nije -1
			indices[o] = indices[o - 1];
		}
		indices[o].neig = k;
		indices[0].check = 0;
		indices[o].udlr = c;
		for (o = k; o > a; o--) { //za svaki sledeci el u edg poveca indeks na ind za 1
			edges[o].edg++;
		}
		
		return 1; //ako je ubacio granu, nasao suseda
	}
	else return 0; // ako nije
}
int insert_node(eledg *edges, elind*indices, int *k,int**mat) { //k postaje indeks sledeceg cvora onog koga ubacujemo
	printf("\nUnestite koordinate novog elementa\n");
	int i, j;
	scanf("%d%d", &i, &j);
	mat[i][j] = *k;
	elind arr[4]; //niz suseda
	int br = 0;
	for (int a = 0; a < *k; a++) { //prolazi kroz niz i trazi suseda, a indeks suseda
		if (edges[a].edg != -1) { //ako taj element nije prethodno izbacen ubaci grane sa susedima
			if (ins_edge(edges[a].m.i, edges[a].m.j, edges, indices, i, j - 1, *k, a, 'r')) { arr[br].neig = a; arr[br++].udlr = 'l'; /*u niz ubacuje koji je to sused i njegov indeks*/ }
			if (ins_edge(edges[a].m.i, edges[a].m.j, edges, indices, i, j + 1, *k, a, 'l')) { arr[br].neig = a; arr[br++].udlr = 'r'; }
			if (ins_edge(edges[a].m.i, edges[a].m.j, edges, indices, i - 1, j, *k, a, 'd')) { arr[br].neig = a; arr[br++].udlr = 'u'; }
			if (ins_edge(edges[a].m.i, edges[a].m.j, edges, indices, i + 1, j, *k, a, 'u')) { arr[br].neig = a; arr[br++].udlr = 'd'; }
		}
	}
	for (int a = 0; a < br; a++) { //sve nadjene susede postavljamo kao susede cvora k
		indices[edges[*k].edg + a] = arr[a];
	}
	edges[*k].m.i = i;
	edges[*k].m.j = j;
	edges[*k].check = 0;
	(*k)++; //povecevamo k
	edges[*k].edg = edges[*k - 1].edg + br;//postavljamo novi kraj reda
	
	return 0;
}
int find_edge(int q,int s, elind*indices) {
	while (q != indices[s].neig) s++;
	return s;
}
int del_edge(eledg *edges,elind*indices,int i,int j,int is,int js,int q,int a,int k) {
	if (i == is && j == js && edges[a].edg!=-1 ) {
		int s=find_edge(q,edges[a].edg,indices);
		for (int i = s; i < edges[k].edg-1; i++) {
			indices[i] = indices[i + 1];
		}
		for (int i = a+1; i <= k; i++) {
			if(edges[i].edg!=-1)edges[i].edg-=1;
		}
		return 1;
	}
	else return 0;
}
int del_edges(eledg*edges,elind*indices,int p,int q,int k) {
	int i = edges[q].m.i;
	int j = edges[q].m.j;
	for (int a = 0; a < k; a++) {
		if (del_edge(edges, indices, i - 1, j, edges[a].m.i, edges[a].m.j, q, a, k) && a < q) p--;
		if (del_edge(edges, indices, i + 1, j, edges[a].m.i, edges[a].m.j, q, a, k) && a < q) p--;
		if (del_edge(edges, indices, i, j - 1, edges[a].m.i, edges[a].m.j, q, a, k) && a < q) p--;
		if (del_edge(edges, indices, i, j + 1, edges[a].m.i, edges[a].m.j, q, a, k) && a < q) p--;
	}
	int s = q;
	q++;
	while (q<k && edges[q].edg == -1) q++;
	if (q != k)  {
		int x = edges[q].edg;
		while (x < edges[k].edg) {
			indices[p++] = indices[x++];
			
		}
		s = x - p;
		while (q <= k) {
			if (edges[q].edg != -1) {
				edges[q].edg -= s;
			}
			q++;
			
		}

	}
	return 0;
	
}
int delete_node(eledg *edges, elind *indices, int k,int**mat) {
	int i, j;
	int q=0;
	printf("Unesite cvor koji zelite da obrisete");
	scanf("%d%d", &i, &j);
	while (q < k && (edges[q].m.i != i || edges[q].m.j != j)) q++;//nalazim cvor i j
	int p;
	if (q!= k && edges[q].edg != -1) {
		p = edges[q].edg; // cuvamo indeks prvog suseda cvora koji izbacujemo
	}
	else {
		printf("Pokusavate da izbacite nepostojeci cvor");
		return 1;
	}
	edges[q].edg = -1; //izbacuje cvor iz edg i iz pomocne matrice
	mat[i][j] = -1;
	
	del_edges(edges,indices, p,q,k);
	return 0;

}
int write_scheme(int **mat,int m,int n,cord s, cord t) {
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			switch (mat[i][j]) {
			case -1: putchar('-'); break;
			case -2: putchar('X'); break;
			case -3: putchar('P'); break;
			default: 
				if (i == s.i&&j == s.j) putchar('S');
				else if (i == t.i&&j == t.j) putchar('T');
				else putchar('o');
			}
		}
		putchar('\n');
	}
}
int write_scheme_dat(int **mat, int m, int n, cord s, cord t) {
	char*imed=malloc(30);
	printf("Unesite ime datoteke:\n");
	scanf("%s", imed);
	FILE*dat = fopen(imed, "w");
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			switch (mat[i][j]) { 
			case -1: putc('-', dat); break;
			case -2: putc('X', dat); break;
			case -3: putc('P', dat); break;
			default:
				if (i == s.i&&j == s.j) putc('S',dat);
				else if (i == t.i&&j == t.j) putc('T',dat);
				else putc('o',dat);
			}
		}
		putc('\n',dat);
	}
	fclose(dat);
}
int** delete_graph(eledg *edges,elind*indices,int**mat,int m,int n) {
	free(edges);
	free(indices);
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			mat[i][j] = -1;
		}
	}
	return mat;

}
void write_arr(eledg*edges,elind* indices,int  k) {
	printf("edges:\n");
	for (int y = 0; y < edges[k].edg; y++) printf("%d ", indices[y].neig);
	putchar('\n');
	printf("indices:\n");
	for (int y = 0; y <= k; y++) if (edges[y].edg != -1)printf("%d ", edges[y].edg);
	putchar('\n');
}
int find_s(int i, int j,eledg*edges) {
	int k = 0;
	while (edges[k].edg==-1||edges[k].m.i != i || edges[k].m.j != j) k++;
	return k;

}
int check_abl(int poc,elind*indices,int end,char c) {
	int i;
	for (i = poc; (i < end) && (indices[i].udlr != c); i++);
	if (i < end) return (indices[i].neig);
	else return -1;
}
state horisontal_move(state state,int p,elind*indices,eledg*edges,int**mat,int game) {
	char c;
	int a,b;
	switch (p) {
	case 1: c = 'u'; break;
	case 2: c = 'd'; break;
	case 3: c = 'l'; break;
	case 4: c = 'r'; break;
	}
	if (p < 3 && !state.ori || p>=3 && state.ori) {
		if ((a=check_abl(edges[state.s[p % 2]].edg, indices, edges[state.s[p % 2] + 1].edg, c)) != -1) {
			if (game) {
				mat[edges[state.s[0]].m.i][edges[state.s[0]].m.j] = state.s[0];
				mat[edges[state.s[1]].m.i][edges[state.s[1]].m.j] = state.s[1];
				mat[edges[a].m.i][edges[a].m.j] = -2;
			}
			state.s[0] = a;
			state.s[1] = -1;
			state.pos = 0;
		}
		else {
			if (game) {
				mat[edges[state.s[0]].m.i][edges[state.s[0]].m.j] = state.s[0];
				mat[edges[state.s[1]].m.i][edges[state.s[1]].m.j] = state.s[1];
				
			}
			state.s[0] = -1;
			state.s[1] = -1;
			
		}
	}
	else {
		if (((b = check_abl(edges[state.s[0]].edg, indices, edges[state.s[0] + 1].edg, c)) != -1) && ((a = check_abl(edges[state.s[1]].edg, indices, edges[state.s[1] + 1].edg, c)) != -1)) {
			if (game) {
				mat[edges[a].m.i][edges[a].m.j] = -2;
				mat[edges[b].m.i][edges[b].m.j] = -2;
				mat[edges[state.s[0]].m.i][edges[state.s[0]].m.j] = state.s[0];
				mat[edges[state.s[1]].m.i][edges[state.s[1]].m.j] = state.s[1];
			}
			state.s[0] = b;
			state.s[1] = a;
		}
		else {
			if (game) {
				mat[edges[state.s[0]].m.i][edges[state.s[0]].m.j] = state.s[0];
				mat[edges[state.s[1]].m.i][edges[state.s[1]].m.j] = state.s[1];
				
			}
			state.s[0] = -1;
			state.s[1] = -1;
			
		}
	}
	
	return state;
}
state vertical_move(state state,int p,elind* indices,eledg* edges,int**mat,int game) {
	char c;
	int a, b;
	switch (p) {
	case 1: c = 'u'; break;
	case 2: c = 'd'; break;
	case 3: c = 'l'; break;
	case 4: c = 'r'; break;
	}
	if ((a = check_abl(edges[state.s[0]].edg, indices, edges[state.s[0] + 1].edg, c)) != -1 && (b = check_abl(edges[a].edg, indices, edges[a + 1].edg, c)) != -1) {
		if (game) {
			mat[edges[a].m.i][edges[a].m.j] = -2;
			mat[edges[b].m.i][edges[b].m.j] = -2;
			mat[edges[state.s[0]].m.i][edges[state.s[0]].m.j] = state.s[0];
		}
		if (p % 2) {
			state.s[0] = a;
			state.s[1] = b;
			
		}
		else {
			state.s[0] = b;
			state.s[1] = a;
		}
		state.pos = 1;
		state.ori = (p < 3) ? 0 : 1;
	}
	else {
		if (game) {
			mat[edges[state.s[0]].m.i][edges[state.s[0]].m.j] = state.s[0];
		}
		state.s[0] = -1;
		state.s[1] = -1;
		
	}
	return state;
}

int play(eledg* edges, elind*indices, int k, cord s, cord t, int**mat, int m, int n) {
	state state;
	int p;
	state.pos = 0;//uspravno
	state.s[0] = find_s(s.i, s.j, edges);//id cvora s
	mat[s.i][s.j] = -2;
	write_scheme(mat, m, n, s, t);
	int finish = find_s(t.i, t.j,edges); //id cvora t
	while (1) {
		printf("---------------------------------\n");
		printf("Odaberite pravac:\n");
		printf("1.gore\n2.dole\n3.levo\n4.desno\n");
		printf("---------------------------------\n");
		scanf("%d", &p);
		if (state.pos) {
			state=horisontal_move(state,p,indices,edges,mat,1);
		}
		else {
			state=vertical_move(state,p,indices,edges,mat,1);
		}
		write_scheme(mat, m, n, s, t);
		if (state.s[0] == finish && !state.pos) { mat[t.i][t.j] = finish; return 1; }
		if (state.s[0] == -1) return 0;
		
		
	}
	
}
void insert_Q(state *Q,state staten,int*rear,int *front,int *max) {
	if (*rear == *max - 1) {
		*max += 100;
		state *a = realloc(Q, *max * sizeof(state));
		if (!a) exit(121);
		Q = a;
	}
	else
	{
		if (*front == -1) *front = 0;
		*rear = *rear + 1;
		Q[*rear] = staten;
	}
}
state delete_Q(state *Q,int *front,int*rear) {
	if (*front == -1 || *front > *rear){
		printf("Underflow \n");
		return;
	}
	else{
		return Q[(*front)++];
	}
}
int not_empty_Q(int front,int rear) {
	if (front == -1 || front > rear) return 0;
	else return 1;
}
int convert_way(int way) {
	switch (way) {
	case 1:way = 2; break;
	case 2:way = 1; break;
	case 3:way = 4; break;
	case 4:way = 3; break;
	}
	return way;
}
char code(int a) {
	char c;
	switch (a) {
	case 1: c = 'u'; break;
	case 2: c = 'd'; break;
	case 3: c = 'l'; break;
	case 4: c = 'r'; break;

	}
	return c;
}
int* get_path(eledg*edges,elind*indices, state fstate, int start,int**mat,int m,int n,cord s,cord t,int i) {
	int way = edges[fstate.s[0]].check=i;
	int arr[50];
	arr[49] = way;
	int br = 48;
	way=convert_way(way);
	while (fstate.s[0] != start||fstate.pos) {
		if (fstate.pos) {
			fstate = horisontal_move(fstate, way, indices, edges, mat, 0);
		}
		else {
			fstate = vertical_move(fstate, way, indices, edges, mat, 0);
		}
		mat[edges[fstate.s[0]].m.i][edges[fstate.s[0]].m.j] = -3;
		if(fstate.s[1]!=-1) mat[edges[fstate.s[1]].m.i][edges[fstate.s[1]].m.j] = -3;
		if (fstate.s[1] == -1) {
			way = edges[fstate.s[0]].check;
		}
		else {
			
			int sus = edges[fstate.s[0]].edg;
			while (indices[sus].neig != fstate.s[1]) sus++;
			way = indices[sus].check;
		}
		arr[br--] = way;
		way = convert_way(way);
		
	}
	printf("Trazeni put je:\n");
	for (int i = ++br +1; i < 50; i++) {
		printf("%c ", code(arr[i]));
	}
	printf("\nJedno resenje igre izgleda ovako(slovom p je oznacen put):\n");
	write_scheme(mat, m, n, s, t);
}
int func(eledg*edges,elind*indices,state state1) {
	int sus = edges[state1.s[0]].edg;
	while (indices[sus].neig != state1.s[1]) sus++;
	if (indices[sus].check != 0) return 1;
	else return 0;
}
int find_solution(eledg* edges, elind *indices,cord s,cord t,int**mat,int m,int n) {
	int max = 100;
	state *queque=malloc(max*sizeof(state));
	int rear = -1;
	int front = -1;
	state state,state1;
	int start=state.s[0] = find_s(s.i, s.j, edges);
	state.pos = 0;
	edges[start].check = -1;
	int finish = find_s(t.i, t.j, edges);
	insert_Q(queque,state,&rear,&front,&max);
	while (not_empty_Q(front,rear)) {
		state = delete_Q(queque, &front, &rear);
		for (int i = 1; i < 5; i++) {//pravci kretanja od 1 do 4
			if (state.pos) {
				state1 = horisontal_move(state, i, indices, edges, mat,0);
			}
			else {
				state1 = vertical_move(state, i, indices, edges, mat,0);
			}
			if (state1.s[0] == finish && !state1.pos) { get_path(edges,indices,state1,start,mat,m,n,s,t,i); return 0; }//ubaci dobijanje puta
			if ((state1.s[0] != -1)) { //obidji
				
				if (state1.s[1] == -1) {
					if(edges[state1.s[0]].check == 0){
						insert_Q(queque, state1, &rear, &front, &max);
						edges[state1.s[0]].check = i;
					}
					
				}
				else {
					int sus = edges[state1.s[0]].edg;
					while (indices[sus].neig != state1.s[1]) sus++;
					if (indices[sus].check == 0) {
						indices[sus].check = i;
						insert_Q(queque, state1, &rear, &front, &max);
						sus = edges[state1.s[1]].edg;
						while (indices[sus].neig != state1.s[0]) sus++;
						indices[sus].check = i;
					}
					

					
					
				}
				
				
			}
		}
	}
	printf("\nDo resenja se ne moze doci!\n");
	return 0;

}
int main() {
	int m=0, n=0,b;
	int ch = 0;
	cord s, t;
	int** mat;
	int k;
	eledg *edges = malloc(1000 * sizeof(eledg));
	if (!edges) return -1;
	elind *indices = malloc(1000 * sizeof(elind));
	if (!indices) return -1;
	while (1) {
		
		printf("______________________________________________________________________________________________\n");
		printf("Odaberite jednu od ponudjenih stavki:\n");
		printf("1. ucitavanje mape terena\n");
		printf("2. formiranje grafa\n");
		printf("3. ubacivanje bloka\n");
		printf("4. izbacivanje bloka\n");
		printf("5. ispis mape na stdout\n");
		printf("6. ispis mape u zadatu tekstualnu datoteku\n");
		printf("7. brisanje mape\n");
		printf("8. ispis niza edges i indices \n");
		printf("9. igraj\n");
		printf("10.resavanje igre\n");
		printf("11. kraj programa\n");
		scanf("%d", &b);
		switch (b) {
		case 1: mat = mat_input_form(&m, &n, &s, &t); break; 
		case 2: k = graphform(mat, m, n, edges, indices); break;//kraj edge
		case 3: insert_node(edges, indices, &k, mat); break;//ubacivanje novog cvora
		case 4: delete_node(edges, indices, k, mat); break;
		case 5: write_scheme(mat, m, n, s, t); break;
		case 6: write_scheme_dat(mat, m, n, s, t); break;
		case 7: mat = delete_graph(edges, indices, mat, m, n); break;
		case 8: write_arr(edges, indices, k,s,t); break;
		case 9: if(play(edges, indices, k,s,t,mat,m,n)) printf("->->->->->->WINNER<-<-<-<-<-\n");
				else printf("GAME OVER\n"); break;
		case 10: find_solution(edges, indices, s, t, mat, m, n); ch = 1; break;
		case 11: return 0;
		}




			/*for (int i = 0; i < m; i++) {
				for (int j = 0; j < n; j++) {
					printf("%3d ", mat[i][j]);
				}
				putchar('\n');
			}*/
		

		

		
		
		

		
		
		
		
	}
}