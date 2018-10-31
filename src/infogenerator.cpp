#include <cstdio>
#include <cstring>
#include <fstream>
using namespace std;

struct B{
    char name[55];
    char id[55];
    int  keynum;
    char keyword[5][55];
    int  anum;
    char author[5][55];
    char pub[55];
    char date[55];
};

void writeinfo(struct B *_b)
{
    char _temp[3];
    _temp[1] = '|';_temp[2] = '\0'; 
    FILE *f = fopen(_b->name, "w");
    fwrite(_b->id, sizeof(char), strlen(_b->id), f);
    fwrite(_temp + 1, sizeof(char), 1, f);
    _temp[0] = '0' + _b->keynum;
    fwrite(_temp, sizeof(char), 2, f);
    for(int i = 0; i < _b->keynum; i++){
        fwrite(_b->keyword[i], sizeof(char), strlen(_b->keyword[i]), f);
        fwrite(_temp + 1, sizeof(char), 1, f);
    }
    _temp[0] = '0' + _b->anum;
    fwrite(_temp, sizeof(char), 2, f);
    for(int i = 0; i < _b->anum; i++){
        fwrite(_b->author[i], sizeof(char), strlen(_b->author[i]), f);
        fwrite(_temp + 1, sizeof(char), 1, f);
    }
    fwrite(_b->pub, sizeof(char), strlen(_b->pub), f);
    fwrite(_temp + 1, sizeof(char), 1, f);
    fwrite(_b->date, sizeof(char), strlen(_b->date), f);
    fclose(f);
    return;
}

void entry()
{
    struct B _b;
    int _temp;
    while(1){
        printf("--------PLEASE PUTIN THE BOOK NAME----------\n");
        scanf("%s", _b.name);
        printf("--------PLEASE PUTIN THE BOOK ID----------\n");
        scanf("%s", _b.id);
        printf("--------PLEASE PUTIN THE NUM OF KEY WORDS--------\n");
        scanf("%d", &_temp);
        while(_temp < 0 || _temp > 5){
            printf("--------PLEASE PUTIN THE NUM OF KEY WORDS--------\n");
            scanf("%d", &_temp);
        }
        _b.keynum = _temp;
        for(int i = 0; i < _temp; i++){
            scanf("%s", _b.keyword[i]);
        }

        printf("--------PLEASE PUTIN THE NUM OF AUTHORS--------\n");
        scanf("%d", &_temp);
        while(_temp < 0 || _temp > 5){
            printf("--------PLEASE PUTIN THE NUM OF AUTHORS--------\n");
            scanf("%d", &_temp);
        }
        _b.anum = _temp;
        for(int i = 0; i < _temp; i++){
            scanf("%s", _b.author[i]);
        }
        printf("--------PLEASE PUTIN THE NUM OF AUTHORS--------\n");
        scanf("%s", _b.pub);
        printf("--------PLEASE PUTIN THE NUM OF AUTHORS--------\n");
        scanf("%s", _b.date);
        
        writeinfo(&_b);

        printf("GO ON?\n");
        scanf("%d", &_temp);
        if(_temp == 0)break;
    }
    return;
}

int main(){
    entry();
    return 0;
}
