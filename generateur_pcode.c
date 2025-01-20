#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLEINDEX 100
#define TAILLEMEM 100
#define TAILLECODE 1000
#define MAX_PARAMS 10
#define MAX_NAME_LENGTH 20

#ifdef _WIN32
#define STRCASECMP stricmp
#else
#define STRCASECMP strcasecmp
#endif

typedef enum {
    ID_TOKEN, PROGRAM_TOKEN, CONST_TOKEN, VAR_TOKEN,
    BEGIN_TOKEN, END_TOKEN, IF_TOKEN, THEN_TOKEN,
    WHILE_TOKEN, DO_TOKEN, READ_TOKEN, WRITE_TOKEN,
    PV_TOKEN, PT_TOKEN, PLUS_TOKEN, MOINS_TOKEN,
    MULT_TOKEN, DIV_TOKEN, VIR_TOKEN, AFF_TOKEN,
    INF_TOKEN, INFEG_TOKEN, SUP_TOKEN, SUPEG_TOKEN,
    DIFF_TOKEN, PO_TOKEN, PF_TOKEN, EOF_TOKEN,
    NUM_TOKEN, ERREUR_TOKEN, FIN_TOKEN, EG_TOKEN,
    REPEAT_TOKEN, UNTIL_TOKEN, FOR_TOKEN, ELSE_TOKEN,
    CASE_TOKEN, OF_TOKEN, INTO_TOKEN, DOWNTO_TOKEN,STRING_TOKEN, STR_LITERAL_TOKEN,
    DDOT_TOKEN, PROCEDURE_TOKEN, CALL_TOKEN , FUNCTION_TOKEN,RETURN_TOKEN
} CODES_LEX;

typedef enum {
    ID_ERR, PROGRAM_ERR, CONST_ERR, VAR_ERR,
    BEGIN_ERR, END_ERR, IF_ERR, THEN_ERR,
    WHILE_ERR, DO_ERR, READ_ERR, WRITE_ERR,
    PV_ERR, PT_ERR, PLUS_ERR, MOINS_ERR,
    MULT_ERR, DIV_ERR, VIR_ERR, AFF_ERR,
    INF_ERR, INFEG_ERR, SUP_ERR, SUPEG_ERR,
    DIFF_ERR, PO_ERR, PF_ERR, EOF_ERR,
    NUM_ERR, ERREUR_ERR, FIN_ERR, EG_ERR,
    CONST_VAR_BEGIN_ERR, VAR_BEGIN_ERR, REPEAT_ERR, UNTIL_ERR,
    FOR_ERR, ELSE_ERR, CASE_ERR, OF_ERR,
    INTO_ERR, DOWNTO_ERR, DDOT_ERR, DD_ERR,INT_TOKEN, REAL_TOKEN, BOOL_TOKEN,TYPE_ERR,
    ND_ERR, ID_NUM_PO_ERR,INST_PCODE_ERR, STRING_ERR, STR_LITERAL_ERR,PROCEDURE_ERR,     // Error in procedure declaration
    CALL_ERR,PARAM_ERR, FUNCTION_ERR, RETURN_ERR
} CODES_ERR;

FILE *fichier,*FICH_SORTIE;
char Car_Cour;

void PROGRAM();
void BLOCK();
void CONSTS();
void VARS();
void INSTS();
void INST();
void AFFEC();
void SI();
void REPETER();
void POUR();
void TANTQUE();
void ECRIRE();
void LIRE();
void COND();
void RELOP();
void EXPR();
void ADDOP();
void TERM();
void MULOP();
void FACT();
void CAS();

void Lire_Car();
void Erreur(CODES_ERR code);
void Test_Symbole(CODES_LEX cl, CODES_ERR COD_ERR);
void Sym_Suiv();
void lire_mot();
void lire_nombre();

typedef struct
{
    CODES_LEX CODE;
    char NOM[20];
} TSym_Cour;

TSym_Cour SYM_COUR;

typedef struct {
    char paramName[MAX_NAME_LENGTH];  // Parameter name
    int isReal;       // Is parameter a real number
    int isString;     // Is parameter a string
    int isReference;  // Is parameter passed by reference
} PARAM_INFO;



typedef enum {TPROG, TVAR,TCONST,TPROC,TFUNC} TypeIDF;
typedef struct {
    TypeIDF TIDF;
    char NOM[20];
    int value;

} T_IDF;

T_IDF TAB_IDFS[255];
int NBR_IDFS=0;
int isReadActivated = 0;
int isProcActivated = 0;
int isFuncActivated = 0;

typedef enum
{
    RPROG,
    RCONST,
    RVAR,
    RBEGIN
} Region;
Region current_reg = RPROG;

//FIN

void Check()
{
    int r = 0;
    int i;
    switch (current_reg)
    {
    case RPROG:
        for (i = 0; i < NBR_IDFS; ++i)
        {
            if (strcmp(SYM_COUR.NOM, TAB_IDFS[i].NOM) == 0)
            {
                r = 1;
                break;
            }
        }

        if (r == 1)
        {
            if (TPROG == TAB_IDFS[i].TIDF)
            {

                printf("%s ----> Erreur:  Le ID du programme ne peut etre utilise dans le programme.\n", SYM_COUR.NOM);

                exit(EXIT_FAILURE);
            }
            else
            {

                printf("%s ----> Erreur: PAS DE DOUBLE DECLARATIONS.\n", SYM_COUR.NOM);

                exit(EXIT_FAILURE);
            }
        }
        strcpy(TAB_IDFS[NBR_IDFS].NOM, SYM_COUR.NOM);
        TAB_IDFS[NBR_IDFS].TIDF = TPROG;
        NBR_IDFS++;

        break;
    case RCONST:
        for (i = 0; i < NBR_IDFS; ++i)
        {
            if (strcmp(SYM_COUR.NOM, TAB_IDFS[i].NOM) == 0)
            {
                r = 1;
                break;
            }
        }

        if (r == 1)
        {
            if (TPROG == TAB_IDFS[i].TIDF)
            {
                printf("%s ----> Erreur:  Le ID du programme ne peut etre utilise dans le programme.", SYM_COUR.NOM);
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("%s ----> Erreur: PAS DE DOUBLE DECLARATIONS.", SYM_COUR.NOM);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            strcpy(TAB_IDFS[NBR_IDFS].NOM, SYM_COUR.NOM);
            TAB_IDFS[NBR_IDFS].TIDF = TCONST;
            NBR_IDFS++;
        }

        break;

    case RVAR:
        for (i = 0; i < NBR_IDFS; ++i)
        {
            if (strcmp(SYM_COUR.NOM, TAB_IDFS[i].NOM) == 0)
            {
                r = 1;
                break;
            }
        }

        if (r == 1)
        {
            if (TPROG == TAB_IDFS[i].TIDF)
            {

                printf("%s ----> Erreur:  Le ID du programme ne peut etre utilise dans le programme.\n", SYM_COUR.NOM);

                exit(EXIT_FAILURE);
            }
            else
            {

                printf("%s ----> Erreur: PAS DE DOUBLE DECLARATIONS.\n", SYM_COUR.NOM);

                exit(EXIT_FAILURE);
            }
        }
        else
        {
        strcpy(TAB_IDFS[NBR_IDFS].NOM, SYM_COUR.NOM);

        //TAB_IDFS[NBR_IDFS].TIDF = (current_reg == RCONST) ? TCONST : TVAR;

        TAB_IDFS[NBR_IDFS].TIDF = TVAR;
                NBR_IDFS++;
            }

        break;

    case RBEGIN:
        for (i = 0; i < NBR_IDFS; ++i)
        {
            if (strcmp(SYM_COUR.NOM, TAB_IDFS[i].NOM) == 0)
            {
                r = 1;
                break;
            }
        }

        if (r == 1)
        {
            if (TPROG == TAB_IDFS[i].TIDF)
            {
                printf("%s ----> Erreur:  Le ID du programme ne peut etre utilise dans le programme.\n", SYM_COUR.NOM);

                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf("%s ----> Erreur:  Tous les symboles doivent etre deja declares.\n", SYM_COUR.NOM);
            exit(EXIT_FAILURE);
        }
        if (isReadActivated)
        {
            isReadActivated = 0;
            if (TCONST == TAB_IDFS[i].TIDF)
            {
                printf("%s ----> Erreur:  Une constante ne peut changer de valeur dans le programme.\n", SYM_COUR.NOM);
                exit(EXIT_FAILURE);
            }
        }

        break;

    default:
        break;
    }

}

void checkifConst(){

    for (int i = 0; i < NBR_IDFS; ++i)
        {
            if ((strcmp(SYM_COUR.NOM, TAB_IDFS[i].NOM) == 0)&&(TAB_IDFS[i].TIDF == TCONST))
            {
                printf("%s ----> Erreur:  Une constante ne peut changer de valeur dans le programme.", SYM_COUR.NOM);
                exit(EXIT_FAILURE);
            }
        }
}

typedef struct {
    char NOM[MAX_NAME_LENGTH];  // Symbol name
    CODES_LEX CLASSE;           // Symbol class (variable, procedure, etc.)
    int ADRESSE;                // Memory address or offset

    // Type information
    int isReal;       // Is real number
    int isString;     // Is string
    int strLength;    // String max length

    // Procedure-specific information
    int isProcedure;  // Is this a procedure
    int paramCount;   // Number of parameters
    PARAM_INFO parameters[MAX_PARAMS];  // Parameter details

    // Scope and variable information
    int localVarCount;  // Number of local variables
    int scopeLevel;
    int isFunction;       // Is this a function
    int returnType;       // Return type (INT, REAL, STRING)
    int hasReturnValue;    // Nesting level of the symbol
} SYMBOLE;

SYMBOLE TABLESYM[TABLEINDEX];

int OFFSET = -1;
int IND_DER_SYM_ACC = 0;
int PC = 0;
int SP = -1;
int index_Mots = 0;
int choice;
int IND_BZE;
int LABEL_BRN;
int AVANT_BEGIN = 1;

int MEM[TAILLEMEM];

typedef enum {
    ADD, SUB, MUL, DIV, EQL, NEQ, GTR,
    LSS, GEQ, LEQ, PRN, INN, INT, LDI, LDA, LDV,
    STO, BRN, BZE, HLT, REAL_ADD, REAL_SUB, REAL_MUL, REAL_DIV,  // Real number operations
    REAL_TO_INT, INT_TO_REAL,LDS,STR, CALL,              // Call a procedure
    RETOUR,            // Return from procedure
    INIT_PROC,         // Initialize procedure frame
    PARAM,             // Push parameter
    PARAM_REF,       // String storage instruction
    PRNS,           // Appel de procédure

} MNEMONIQUES;

typedef struct {
    MNEMONIQUES MNE;
    union {
        int SUITE;
        char STR_VAL[256];  // For storing string literals
    };
} INSTRUCTION;

INSTRUCTION PCODE[TAILLECODE];
TSym_Cour sym,sym2,sym3;

void Lire_Car() {
    Car_Cour = fgetc(fichier);
}


void lire_nombre() {
    char nombre[20];
    int indice = 0;
    int hasDecimal = 0;
    int isNegative = 0;

    // Check for unary minus
    if (Car_Cour == '-') {
        isNegative = 1;
        nombre[indice++] = Car_Cour;
        Lire_Car();
    }

    // Read the first digit
    nombre[indice++] = Car_Cour;
    Lire_Car();

    // Read remaining digits and one optional decimal point
    while (isdigit(Car_Cour) || (Car_Cour == '.' && !hasDecimal)) {
        if (Car_Cour == '.') {
            hasDecimal = 1;
        }
        nombre[indice++] = Car_Cour;
        Lire_Car();
    }

    nombre[indice] = '\0';
    SYM_COUR.CODE = NUM_TOKEN;
    strcpy(SYM_COUR.NOM, nombre);
}

void lire_mot(){
    char mot[20];
    int indice = 0;
    mot[indice++] = Car_Cour;
    Lire_Car();
    while (isalpha(Car_Cour) || isdigit(Car_Cour)){
        mot[indice++] = Car_Cour;
        Lire_Car();
    }
    mot[indice] = '\0';
    if (STRCASECMP(mot, "program") == 0){
        SYM_COUR.CODE = PROGRAM_TOKEN;
    }
    else if (STRCASECMP(mot, "const") == 0){
        SYM_COUR.CODE = CONST_TOKEN;
    }
    else if (STRCASECMP(mot, "var") == 0){
        SYM_COUR.CODE = VAR_TOKEN;
    }
    else if (STRCASECMP(mot, "begin") == 0){
        SYM_COUR.CODE = BEGIN_TOKEN;
    }
    else if (STRCASECMP(mot, "end") == 0){
        SYM_COUR.CODE = END_TOKEN;
    }
    else if (STRCASECMP(mot, "if") == 0){
        SYM_COUR.CODE = IF_TOKEN;
    }
    else if (STRCASECMP(mot, "then") == 0){
        SYM_COUR.CODE = THEN_TOKEN;
    }
    else if (STRCASECMP(mot, "while") == 0){
        SYM_COUR.CODE = WHILE_TOKEN;
    }
    else if (STRCASECMP(mot, "do") == 0){
        SYM_COUR.CODE = DO_TOKEN;
    }
    else if (STRCASECMP(mot, "read") == 0){
        SYM_COUR.CODE = READ_TOKEN;
         isReadActivated=1;
    }
    else if (STRCASECMP(mot, "write") == 0){
        SYM_COUR.CODE = WRITE_TOKEN;
    }
    else if (STRCASECMP(mot, "else") == 0){
        SYM_COUR.CODE = ELSE_TOKEN;
    }
    else if (STRCASECMP(mot, "repeat") == 0){
        SYM_COUR.CODE = REPEAT_TOKEN;
    }
    else if (STRCASECMP(mot, "until") == 0){
        SYM_COUR.CODE = UNTIL_TOKEN;
    }
    else if (STRCASECMP(mot, "for") == 0){
        SYM_COUR.CODE = FOR_TOKEN;
    }
    else if (STRCASECMP(mot, "downto") == 0) {
        SYM_COUR.CODE = DOWNTO_TOKEN;
    }
    else if (STRCASECMP(mot, "to") == 0) {
        SYM_COUR.CODE = INTO_TOKEN;
    }
    else if (STRCASECMP(mot, "case") == 0){
        SYM_COUR.CODE = CASE_TOKEN;
    }
    else if (STRCASECMP(mot, "of") == 0){
        SYM_COUR.CODE = OF_TOKEN;
    }
    else if (STRCASECMP(mot, "integer") == 0){
        SYM_COUR.CODE = INT_TOKEN;
    }
    else if (STRCASECMP(mot, "real") == 0){
        SYM_COUR.CODE = REAL_TOKEN;
    }
    else if (STRCASECMP(mot, "boolean") == 0){
        SYM_COUR.CODE = BOOL_TOKEN;
    }
    else if (STRCASECMP(mot, "String") == 0) {
        SYM_COUR.CODE = STRING_TOKEN;
    }
    else if (STRCASECMP(mot, "procedure") == 0) {
        SYM_COUR.CODE = PROCEDURE_TOKEN;
    }
    else if (STRCASECMP(mot, "call") == 0) {
        SYM_COUR.CODE = CALL_TOKEN;
    }
    else if (STRCASECMP(mot, "function") == 0){
        SYM_COUR.CODE = FUNCTION_TOKEN;
    }
    else if (STRCASECMP(mot, "return") == 0){
        SYM_COUR.CODE = RETURN_TOKEN;
    }
    else{
        SYM_COUR.CODE = ID_TOKEN;
        strcpy(SYM_COUR.NOM, mot);
        Check();
        return;
    }
    strcpy(SYM_COUR.NOM, mot);
}
void Sym_Suiv() {
    // Skip whitespace
    while (Car_Cour == ' ' || Car_Cour == '\n' || Car_Cour == '\t') {
        Lire_Car();
    }

    if (Car_Cour == '\'') {  // String literal handling first
        char str[256];
        int i = 0;
        Lire_Car();  // Skip opening quote

        // Read everything between quotes, regardless of content
        while (Car_Cour != '\'' && Car_Cour != EOF && i < 255) {
            str[i++] = Car_Cour;
            Lire_Car();
        }

        str[i] = '\0';  // Null terminate the string

        if (Car_Cour == '\'') {
            Lire_Car();  // Skip closing quote
            SYM_COUR.CODE = STR_LITERAL_TOKEN;
            strcpy(SYM_COUR.NOM, str);
            printf("Symbol: '%s'\n", SYM_COUR.NOM);  // Print with quotes to show it's a string
            return;
        } else {
            SYM_COUR.CODE = ERREUR_TOKEN;
            strcpy(SYM_COUR.NOM, "unterminated string");
            printf("Symbol: ERROR - unterminated string\n");
            return;
        }
    }

    // Handle identifiers and keywords
    if (isalpha(Car_Cour)) {
        lire_mot();
    }
    // Handle numbers
    else if (isdigit(Car_Cour)) {
        lire_nombre();
    }
    // Handle special characters and operators
    else {
        char s[3];  // Increased size to handle two-character operators
        s[0] = Car_Cour;

        switch (Car_Cour) {
        case ';':
            s[1] = '\0';
            SYM_COUR.CODE = PV_TOKEN;
            Lire_Car();
            break;

        case '+':
            s[1] = '\0';
            SYM_COUR.CODE = PLUS_TOKEN;
            Lire_Car();
            break;

        case '-':
            s[1] = '\0';
            SYM_COUR.CODE = MOINS_TOKEN;
            Lire_Car();
            break;

        case '*':
            s[1] = '\0';
            SYM_COUR.CODE = MULT_TOKEN;
            Lire_Car();
            break;

        case '/':
            s[1] = '\0';
            SYM_COUR.CODE = DIV_TOKEN;
            Lire_Car();
            break;

        case ',':
            s[1] = '\0';
            SYM_COUR.CODE = VIR_TOKEN;
            Lire_Car();
            break;

        case ':':
            Lire_Car();
            if (Car_Cour == '='){
                s[1] = '=';
                s[2] = '\0';
                SYM_COUR.CODE = AFF_TOKEN;
                Lire_Car();
            }
            else{
                s[1] = '\0';
                SYM_COUR.CODE = DDOT_TOKEN;
            }
            break;

        case '<':
            Lire_Car();
            if (Car_Cour == '='){
                s[1] = '=';
                s[2] = '\0';
                SYM_COUR.CODE = INFEG_TOKEN;
                Lire_Car();
            }
            else if (Car_Cour == '>'){
                s[1] = '=';
                s[2] = '\0';
                SYM_COUR.CODE = DIFF_TOKEN;
                Lire_Car();
            }
            else{
                s[1] = '\0';
                SYM_COUR.CODE = INF_TOKEN;
            }
            break;

        case '>':
            Lire_Car();
            if (Car_Cour == '='){
                s[1] = '=';
                s[2] = '\0';
                SYM_COUR.CODE = SUPEG_TOKEN;
                Lire_Car();
            }
            else{
                s[1] = '\0';
                SYM_COUR.CODE = SUP_TOKEN;
            }
            break;

        case '(':
            s[1] = '\0';
            SYM_COUR.CODE = PO_TOKEN;
            Lire_Car();
            break;
        case '=':
            s[1] = '\0';
            SYM_COUR.CODE = EG_TOKEN;
            Lire_Car();
            break;

        case ')':
            s[1] = '\0';
            SYM_COUR.CODE = PF_TOKEN;
            Lire_Car();
            break;

        case '.':
            s[1] = '\0';
            SYM_COUR.CODE = PT_TOKEN;
            Lire_Car();
            break;

        case EOF:
            s[1] = '\0';
            SYM_COUR.CODE = EOF_TOKEN;
            break;

        default:
            s[1] = '\0';
            SYM_COUR.CODE = ERREUR_TOKEN;
            Lire_Car();
        }
        strcpy(SYM_COUR.NOM,s);
    }
    printf("Symbol: %s\n", SYM_COUR.NOM);
}

void Erreur(CODES_ERR code){
    printf("Erreur: %d\n", code);
    printf("Current Token: %d\n", SYM_COUR.CODE);
    printf("Current Lexeme: %s\n", SYM_COUR.NOM);
    exit(EXIT_FAILURE);
}

int RechercherSym(char mot[20]) {
    printf("Searching for symbol: %s\n", mot);  // Debug print
    for (int i = 0; i < index_Mots; i++) {
        printf("Comparing with %s\n", TABLESYM[i].NOM);  // Debug print
        if (strcmp(TABLESYM[i].NOM, mot) == 0) {
            printf("Found at index %d\n", i);  // Debug print
            return i;
        }
    }
    printf("Symbol not found\n");  // Debug print
    return -1;
}

void AJOUTER(){
    TABLESYM[IND_DER_SYM_ACC].ADRESSE = ++OFFSET;
    TABLESYM[IND_DER_SYM_ACC].CLASSE = ID_TOKEN;
    strcpy(TABLESYM[IND_DER_SYM_ACC].NOM,sym.NOM);
    index_Mots++;
}

void Codage_Lex(char mot[20]) {
    int indice_token = -1;

    indice_token = RechercherSym(mot);

    if (indice_token != -1) {
        if ((AVANT_BEGIN == 1) && (indice_token > 10)) {
            Erreur(DD_ERR);
        } else {
            IND_DER_SYM_ACC = indice_token;
        }
    } else {
        if (AVANT_BEGIN == 1) {
            IND_DER_SYM_ACC = index_Mots;
            AJOUTER();
        } else {
            Erreur(ND_ERR);
        }
    }
}


void Test_Symbole(CODES_LEX cl, CODES_ERR COD_ERR){
    if (SYM_COUR.CODE == cl){
        Sym_Suiv();
    }
    else{
        Erreur(COD_ERR);
    }
}



void GENERER2(MNEMONIQUES M, int A) {
    if (PC == TAILLECODE) {
        printf("ERROR: PC is equal to TAILLECODE.\n");
    } else {
        PC = PC + 1;
        PCODE[PC].MNE = M;
        PCODE[PC].SUITE = A;
    }
}

void GENERER1(MNEMONIQUES M) {
    if (PC == TAILLECODE) {
        printf("ERROR: PC is equal to TAILLECODE.\n");
    }else {
        GENERER2(M,-1);
    }
}




void BLOCK() {
    OFFSET = -1;
    current_reg = RCONST;
    CONSTS();
    current_reg = RVAR;
    VARS();
    current_reg = RBEGIN;

    // Handle function and procedure declarations BEFORE generating the initial INT instruction
    while (SYM_COUR.CODE == PROCEDURE_TOKEN || SYM_COUR.CODE == FUNCTION_TOKEN) {
        if (SYM_COUR.CODE == PROCEDURE_TOKEN) {
            PROCEDURE_DECL();
        } else {
            FUNCTION_DECL();
        }
    }

    // Generate the INT instruction AFTER all function and procedure declarations
    PCODE[0].MNE = INT;
    PCODE[0].SUITE = OFFSET + 1;

    INSTS();
}

void PROGRAM(){
    Test_Symbole(PROGRAM_TOKEN, PROGRAM_ERR);
    Test_Symbole(ID_TOKEN, ID_ERR);
    Test_Symbole(PV_TOKEN, PV_ERR);
    BLOCK();
    GENERER1(HLT);
    Test_Symbole(PT_TOKEN, PT_ERR);
}

void AFFEC() {
    sym = SYM_COUR;
    checkifConst();
    Test_Symbole(ID_TOKEN, ID_ERR);

    Codage_Lex(sym.NOM);

    // If this is a function name (for return value assignment)
    if (TABLESYM[IND_DER_SYM_ACC].isFunction) {
        // Explicitly handle function return value assignment
        GENERER2(LDA, TABLESYM[IND_DER_SYM_ACC].ADRESSE);
        Test_Symbole(AFF_TOKEN, AFF_ERR);

        EXPR();

        // Mark that return value has been set
        TABLESYM[IND_DER_SYM_ACC].hasReturnValue = 1;

        GENERER1(STO);
        return;
    }

    // Regular variable assignment
    Test_Symbole(AFF_TOKEN, AFF_ERR);

    if (TABLESYM[IND_DER_SYM_ACC].isString) {
        if (SYM_COUR.CODE == STR_LITERAL_TOKEN) {
            // Get base address from symbol table
            int baseAddr = TABLESYM[IND_DER_SYM_ACC].ADRESSE;

            // Store string length at base address
            GENERER2(LDA, baseAddr);
            GENERER2(LDI, strlen(SYM_COUR.NOM));
            GENERER1(STO);

            // Store each character starting at baseAddr + 1
            for (int i = 0; i < strlen(SYM_COUR.NOM); i++) {
                GENERER2(LDA, baseAddr + i + 1);
                GENERER2(LDI, SYM_COUR.NOM[i]);
                GENERER1(STO);
            }

            Sym_Suiv();
        } else {
            Erreur(STR_LITERAL_ERR);
        }
    } else {
        // Check if this is a function call
        if (SYM_COUR.CODE == PO_TOKEN) {
            // Find the function
            int funcIndex = RechercherSym(sym.NOM);
            if (funcIndex != -1 && TABLESYM[funcIndex].isFunction) {
                Sym_Suiv();  // Move past opening parenthesis

                int paramCount = 0;
                while (SYM_COUR.CODE != PF_TOKEN) {
                    if (paramCount >= TABLESYM[funcIndex].paramCount) {
                        Erreur(PARAM_ERR);
                        return;
                    }

                    PARAM_INFO* currentParam = &TABLESYM[funcIndex].parameters[paramCount];

                    // Handle parameter based on passing mode
                    if (currentParam->isReference) {
                        // Reference parameter (VAR)
                        sym = SYM_COUR;
                        Test_Symbole(ID_TOKEN, ID_ERR);

                        int varIndex = RechercherSym(sym.NOM);
                        if (varIndex == -1) {
                            Erreur(ND_ERR);
                            return;
                        }

                        // Type checking for reference parameters
                        if (currentParam->isReal != TABLESYM[varIndex].isReal ||
                            currentParam->isString != TABLESYM[varIndex].isString) {
                            Erreur(TYPE_ERR);
                            return;
                        }

                        GENERER2(LDA, TABLESYM[varIndex].ADRESSE);
                        GENERER1(PARAM_REF);
                    } else {
                        // Value parameter - create a local copy
                        EXPR();
                        GENERER1(PARAM);
                    }

                    paramCount++;

                    if (SYM_COUR.CODE == VIR_TOKEN) {
                        Sym_Suiv();
                        if (SYM_COUR.CODE == PF_TOKEN) {
                            // Extra comma at end of parameter list
                            Erreur(PARAM_ERR);
                            return;
                        }
                    } else if (SYM_COUR.CODE != PF_TOKEN && paramCount < TABLESYM[funcIndex].paramCount) {
                        // Missing comma between parameters
                        Erreur(VIR_ERR);
                        return;
                    }
                }

                // Check if we got all required parameters
                if (paramCount < TABLESYM[funcIndex].paramCount) {
                    Erreur(PARAM_ERR);
                    return;
                }

                Test_Symbole(PF_TOKEN, PF_ERR);

                // Generate procedure call
                GENERER2(CALL, TABLESYM[funcIndex].ADRESSE);

                // Load the return value
                GENERER2(LDA, TABLESYM[funcIndex].ADRESSE);
                GENERER1(LDV);

                // Store the function return value
                GENERER1(STO);
            } else {
                Erreur(ND_ERR);
            }
        } else {
            EXPR();
            GENERER1(STO);
        }
    }
}

void EXPR() {
    TERM();
    while ((SYM_COUR.CODE == PLUS_TOKEN) || (SYM_COUR.CODE == MOINS_TOKEN)) {
        sym = SYM_COUR;
        Sym_Suiv();
        TERM();
        if (sym.CODE == PLUS_TOKEN) {
            GENERER1(ADD);
        } else {
            GENERER1(SUB);
        }
    }
}



void TERM() {
    FACT();
    while ((SYM_COUR.CODE==MULT_TOKEN) || (SYM_COUR.CODE==DIV_TOKEN)) {
        sym = SYM_COUR;
        Sym_Suiv();
        FACT();

        // Check if either operand is a real number
        int isRealOperation =
            (TABLESYM[IND_DER_SYM_ACC].isReal) ||
            (index_Mots > 0 && TABLESYM[index_Mots-1].isReal);

        if (isRealOperation) {
            if(sym.CODE==MULT_TOKEN) {
                GENERER1(MUL);           // Multiply the operands
                GENERER2(LDI, 100);      // Load scaling factor
                GENERER1(DIV);           // Divide by scaling factor
            }
            else {
                GENERER1(DIV);           // Divide the operands
                GENERER2(LDI, 100);      // Load scaling factor
                GENERER1(MUL);           // Multiply by scaling factor
            }
        } else {
            // For integers, perform normal multiplication/division without scaling
            if(sym.CODE==MULT_TOKEN) {
                GENERER1(MUL);
            }
            else {
                GENERER1(DIV);
            }
        }
    }
}

void FACT() {
    switch (SYM_COUR.CODE) {
        case ID_TOKEN: {
            int symIndex = RechercherSym(SYM_COUR.NOM);
            if (symIndex == -1) {
                Erreur(ND_ERR);
                return;
            }
            GENERER2(LDA, TABLESYM[symIndex].ADRESSE);
            GENERER1(LDV);
            Sym_Suiv();
            break;
        }

        case NUM_TOKEN: {
    char *endptr;
    double value = strtod(SYM_COUR.NOM, &endptr);

    // Check if we're currently processing an integer variable/parameter
    if (IND_DER_SYM_ACC >= 0 && !TABLESYM[IND_DER_SYM_ACC].isReal) {
        // For integers, use the value directly
        GENERER2(LDI, (int)value);
    } else {
        // For reals, use fixed-point
        int fixedPoint = (int)(value * 100);
        GENERER2(LDI, fixedPoint);
    }
    Sym_Suiv();
    break;
}

        case PO_TOKEN:
            Sym_Suiv();
            EXPR();
            Test_Symbole(PF_TOKEN, PF_ERR);
            break;

        default:
            Erreur(ID_NUM_PO_ERR);
            break;
    }
}

// Helper function to handle string literals
void HandleStringLiteral() {
    // Calculate base address and advance offset
    int baseAddr = OFFSET + 1;
    int strLength = strlen(SYM_COUR.NOM);
    OFFSET += strLength + 1;

    // Store string length
    GENERER2(LDA, baseAddr);
    GENERER2(LDI, strLength);
    GENERER1(STO);

    // Store string characters
    for (int i = 0; i < strLength; i++) {
        GENERER2(LDA, baseAddr + i + 1);
        GENERER2(LDI, SYM_COUR.NOM[i]);
        GENERER1(STO);
    }

    // Print the string
    GENERER2(LDA, baseAddr);
    GENERER1(PRN);

    Sym_Suiv();
}

void ECRIRE() {
    Test_Symbole(WRITE_TOKEN, WRITE_ERR);
    Test_Symbole(PO_TOKEN, PO_ERR);

    // Handle first expression
    if (SYM_COUR.CODE == STR_LITERAL_TOKEN) {
        HandleStringLiteral();
    } else {
        EXPR();

        // Check if the last processed variable/value is an integer (not a real)
        if (!TABLESYM[IND_DER_SYM_ACC].isReal) {
            // If it's an integer, divide by 100 before printing
            GENERER2(LDI, 100);
            GENERER1(DIV);
        }

        GENERER1(PRN);
    }

    // Handle additional expressions
    while (SYM_COUR.CODE == VIR_TOKEN) {
        Sym_Suiv();
        if (SYM_COUR.CODE == STR_LITERAL_TOKEN) {
            HandleStringLiteral();
        } else {
            EXPR();

            // Check if the last processed variable/value is an integer (not a real)
            if (!TABLESYM[IND_DER_SYM_ACC].isReal) {
                // If it's an integer, divide by 100 before printing
                GENERER2(LDI, 100);
                GENERER1(DIV);
            }

            GENERER1(PRN);
        }
    }

    Test_Symbole(PF_TOKEN, PF_ERR);
}

void LIRE(){
    Test_Symbole(READ_TOKEN, READ_ERR);
    Test_Symbole(PO_TOKEN, PO_ERR);
    sym = SYM_COUR;
    Test_Symbole(ID_TOKEN, ID_ERR);
    Codage_Lex(sym.NOM);
    GENERER2(LDA, TABLESYM[IND_DER_SYM_ACC].ADRESSE);
    GENERER1(INN);
    while (SYM_COUR.CODE==VIR_TOKEN){
        Sym_Suiv();
        sym = SYM_COUR;
        Test_Symbole(ID_TOKEN, ID_ERR);
        Codage_Lex(sym.NOM);
        GENERER2(LDA, TABLESYM[IND_DER_SYM_ACC].ADRESSE);
        GENERER1(INN);
    }
    Test_Symbole(PF_TOKEN, PF_ERR);
}



void RELOP(){
    switch (SYM_COUR.CODE){
    case EG_TOKEN:
        Test_Symbole(EG_TOKEN, EG_ERR);
        choice = 1;
        break;
    case DIFF_TOKEN:
        Test_Symbole(DIFF_TOKEN, DIFF_ERR);
        choice = 2;
        break;
    case INF_TOKEN:
        Test_Symbole(INF_TOKEN, INF_ERR);
        choice = 3;
        break;
    case SUP_TOKEN:
        Test_Symbole(SUP_TOKEN, SUP_ERR);
        choice = 4;
        break;
    case INFEG_TOKEN:
        Test_Symbole(INFEG_TOKEN, INFEG_ERR);
        choice = 5;
        break;
    case SUPEG_TOKEN:
        Test_Symbole(SUPEG_TOKEN, SUPEG_ERR);
        choice = 6;
        break;
    default:
        Erreur(ERREUR_ERR);
        break;
    }
}


void COND(){
    choice = 0;
    EXPR();
    RELOP();
    EXPR();
    switch(choice){
        case 1:
            GENERER1(EQL);
            break;
        case 2:
            GENERER1(NEQ);
            break;
        case 3:
            GENERER1(GTR);
            break;
        case 4:
            GENERER1(GEQ);
            break;
        case 5:
            GENERER1(LSS);
            break;
        case 6:
            GENERER1(LEQ);
            break;
    }
}



void SI(){
    Test_Symbole(IF_TOKEN, IF_ERR);
    COND();
    GENERER2(BZE, PC+1);  // Generate branch if zero (false condition)
    IND_BZE = PC;         // Save the branch instruction index
    Test_Symbole(THEN_TOKEN, THEN_ERR);
    INST();               // Parse then branch

    if (SYM_COUR.CODE == ELSE_TOKEN){
        // If else exists, generate an unconditional branch to skip else
        GENERER2(BRN, PC+1);
        int IND_BRN = PC;

        // Update the previous BZE to point to the else branch
        PCODE[IND_BZE].SUITE = PC+1;

        Sym_Suiv();       // Consume ELSE token
        INST();           // Parse else branch

        // Update the branch to skip else to the end
        PCODE[IND_BRN].SUITE = PC+1;
    } else {
        // No else, just update the BZE to the next instruction
        PCODE[IND_BZE].SUITE = PC+1;
    }
}




void TANTQUE(){
    Test_Symbole(WHILE_TOKEN, WHILE_ERR);
    LABEL_BRN = PC+1;
    COND();
    GENERER1(BZE);
    IND_BZE = PC;
    Test_Symbole(DO_TOKEN, DO_ERR);
    INST();
    GENERER2(BRN, LABEL_BRN);
    PCODE[IND_BZE].SUITE=PC+1;
}


void CONSTS() {
    switch (SYM_COUR.CODE) {
        case CONST_TOKEN: {
            Sym_Suiv();
            sym = SYM_COUR;
            Test_Symbole(ID_TOKEN, ID_ERR);
            Codage_Lex(sym.NOM);
            GENERER2(LDA, TABLESYM[IND_DER_SYM_ACC].ADRESSE);
            Test_Symbole(EG_TOKEN, EG_ERR);
            sym = SYM_COUR;
            Test_Symbole(NUM_TOKEN, NUM_ERR);
            GENERER2(LDI, atoi(sym.NOM));
            GENERER1(STO);
            Test_Symbole(PV_TOKEN, PV_ERR);

            while (SYM_COUR.CODE == ID_TOKEN) {
                sym = SYM_COUR;
                Sym_Suiv();
                Codage_Lex(sym.NOM);
                GENERER2(LDA, TABLESYM[IND_DER_SYM_ACC].ADRESSE);
                Test_Symbole(EG_TOKEN, EG_ERR);
                sym = SYM_COUR;
                Test_Symbole(NUM_TOKEN, NUM_ERR);
                GENERER2(LDI, atoi(sym.NOM));
                GENERER1(STO);
                Test_Symbole(PV_TOKEN, PV_ERR);
            };
            break;
        }
        case VAR_TOKEN: break;
        case BEGIN_TOKEN: break;
        default: Erreur(CONST_VAR_BEGIN_ERR); break;
    }
}

void VARS() {
    switch (SYM_COUR.CODE) {
    case VAR_TOKEN:
        Sym_Suiv();
        do {
            // Create temporary storage for identifiers
            #define MAX_IDS 50
            TSym_Cour id_list[MAX_IDS];
            int id_count = 0;

            // First identifier
            id_list[id_count++] = SYM_COUR;
            Test_Symbole(ID_TOKEN, ID_ERR);

            // Handle multiple identifiers for same type
            while (SYM_COUR.CODE == VIR_TOKEN) {
                Sym_Suiv();
                id_list[id_count++] = SYM_COUR;
                Test_Symbole(ID_TOKEN, ID_ERR);
            }

            Test_Symbole(DDOT_TOKEN, DDOT_ERR);

            // Process type declaration for all collected identifiers
            switch (SYM_COUR.CODE) {
                case INT_TOKEN:
                    for (int i = 0; i < id_count; i++) {
                        strcpy(TABLESYM[index_Mots].NOM, id_list[i].NOM);
                        TABLESYM[index_Mots].CLASSE = ID_TOKEN;
                        TABLESYM[index_Mots].ADRESSE = ++OFFSET;
                        TABLESYM[index_Mots].isReal = 0;
                        TABLESYM[index_Mots].isString = 0;
                        index_Mots++;
                    }
                    Sym_Suiv();
                    break;

                case REAL_TOKEN:
                    for (int i = 0; i < id_count; i++) {
                        strcpy(TABLESYM[index_Mots].NOM, id_list[i].NOM);
                        TABLESYM[index_Mots].CLASSE = ID_TOKEN;
                        TABLESYM[index_Mots].ADRESSE = ++OFFSET;
                        TABLESYM[index_Mots].isReal = 1;
                        TABLESYM[index_Mots].isString = 0;
                        index_Mots++;
                    }
                    Sym_Suiv();
                    break;

                case STRING_TOKEN:
                    for (int i = 0; i < id_count; i++) {
                        strcpy(TABLESYM[index_Mots].NOM, id_list[i].NOM);
                        TABLESYM[index_Mots].CLASSE = ID_TOKEN;
                        TABLESYM[index_Mots].ADRESSE = OFFSET + 1;  // Base address for this string
                        TABLESYM[index_Mots].isReal = 0;
                        TABLESYM[index_Mots].isString = 1;
                        TABLESYM[index_Mots].strLength = 255;
                        OFFSET += 256;  // Allocate space for length + max chars
                        index_Mots++;
                    }
                    Sym_Suiv();
                    break;

                default:
                    Erreur(TYPE_ERR);
                    break;
            }

            Test_Symbole(PV_TOKEN, PV_ERR);
        } while (SYM_COUR.CODE == ID_TOKEN);
        break;

    case BEGIN_TOKEN:
        break;

    default:
        Erreur(VAR_BEGIN_ERR);
        break;
    }
}




void INSTS(){
    if (SYM_COUR.CODE == BEGIN_TOKEN){
        Sym_Suiv();
        INST();
        while (SYM_COUR.CODE == PV_TOKEN){
            Sym_Suiv();
            INST();
        }
        if (SYM_COUR.CODE == END_TOKEN){
            Sym_Suiv();
        }
        else{
            Erreur(END_ERR);
        }
    }
    else{
        Erreur(BEGIN_ERR);
    }
}


void INST() {

        switch (SYM_COUR.CODE) {
        case ID_TOKEN: {
            // Save the current symbol before searching
            sym = SYM_COUR;
            int symIndex = RechercherSym(sym.NOM);
            printf("Processing ID: %s, found at index: %d\n", sym.NOM, symIndex);  // Debug print

            if (symIndex != -1) {
                if (TABLESYM[symIndex].isProcedure) {
                    // This is a procedure call
                    printf("Recognized as procedure call\n");  // Debug print
                    Sym_Suiv();  // Move past procedure name
                    CALL_PROCEDURE(symIndex);
                } else if (TABLESYM[symIndex].isFunction) {
                    // This is an assignment or function call
                    printf("Recognized as function-related operation\n");  // Debug print
                    AFFEC();
                } else {
                    // This is an assignment or other use of the identifier
                    printf("Recognized as assignment\n");  // Debug print
                    AFFEC();
                }
            } else {
                // Symbol not found
                printf("Symbol not found\n");  // Debug print
                Erreur(ND_ERR);
            }
            break;
        }
        case BEGIN_TOKEN:
            INSTS();
            break;
        case IF_TOKEN:
            SI();
            break;
        case WHILE_TOKEN:
            TANTQUE();
            break;
        case WRITE_TOKEN:
            ECRIRE();
            break;
        case READ_TOKEN:
            LIRE();
            break;
        case REPEAT_TOKEN:
            REPETER();
            break;
        case FOR_TOKEN:
            POUR();
            break;
        case CASE_TOKEN:
            CAS();
            break;
        case CALL_TOKEN: {
           Sym_Suiv();
            int symIndex = RechercherSym(SYM_COUR.NOM);
            if (symIndex != -1) {
                if (TABLESYM[symIndex].isProcedure) {
                    CALL_PROCEDURE(symIndex);
                } else if (TABLESYM[symIndex].isFunction) {
                    CALL_FUNCTION(symIndex);
                } else {
                    Erreur(CALL_ERR);
                }
            } else {
                Erreur(ND_ERR);
            }
            break;
        }
        case RETURN_TOKEN:
            RETURN_STMT();
            break;
        default:
            break;
    }
}


void POUR(){
    Test_Symbole(FOR_TOKEN, FOR_ERR);
    sym = SYM_COUR;
    AFFEC();
    switch (SYM_COUR.CODE){
        case DOWNTO_TOKEN:
            sym3 = SYM_COUR;
            Test_Symbole(DOWNTO_TOKEN, DOWNTO_ERR);
            break;
        case INTO_TOKEN:
            sym3 = SYM_COUR;
            Test_Symbole(INTO_TOKEN, INTO_ERR);
            break;
        default:
            Erreur(ERREUR_ERR);
            break;
    }
    sym2 = SYM_COUR;
    Test_Symbole(NUM_TOKEN, NUM_ERR);
    Test_Symbole(DO_TOKEN, DO_ERR);
    LABEL_BRN = PC+1;
    Codage_Lex(sym.NOM);
    GENERER2(LDA, TABLESYM[IND_DER_SYM_ACC].ADRESSE);
    GENERER1(LDV);
    GENERER2(LDI, atoi(sym2.NOM));
    switch (sym3.CODE){
        case DOWNTO_TOKEN:
            GENERER1(GEQ);
            break;
        case INTO_TOKEN:
            GENERER1(LEQ);
            break;
        default:
            break;
    }
    IND_BZE = PC+1;
    GENERER1(BZE);
    INST();
    Codage_Lex(sym.NOM);
    GENERER2(LDA,TABLESYM[IND_DER_SYM_ACC].ADRESSE);
    GENERER2(LDA,TABLESYM[IND_DER_SYM_ACC].ADRESSE);
    GENERER1(LDV);
    GENERER2(LDI,1);
    switch (sym3.CODE){
        case DOWNTO_TOKEN:
            GENERER1(SUB);
            break;
        case INTO_TOKEN:
            GENERER1(ADD);
            break;
        default:
            break;
    }
    GENERER1(STO);
    GENERER2(BRN,LABEL_BRN);
    PCODE[IND_BZE].SUITE = PC+1;
}

void REPETER() {
    int debut_repeat;

    Test_Symbole(REPEAT_TOKEN, REPEAT_ERR);
    debut_repeat = PC + 1;      // Sauvegarder le point de départ

    // Corps du REPEAT
    do {
        INST();
        if (SYM_COUR.CODE == PV_TOKEN) {
            Sym_Suiv();
        }
    } while (SYM_COUR.CODE != UNTIL_TOKEN);

    // Condition de fin
    Test_Symbole(UNTIL_TOKEN, UNTIL_ERR);
    COND();                     // Génère le code pour la condition

    // Branchement vers le début si la condition est fausse
    GENERER2(BZE, debut_repeat);
}

void CAS() {
    int *branch_ends = malloc(100 * sizeof(int)); // Array to store branch end locations
    int branch_count = 0;
    int case_end;
    TSym_Cour case_var;

    Test_Symbole(CASE_TOKEN, CASE_ERR);

    // Save the case variable identifier
    case_var = SYM_COUR;
    Test_Symbole(ID_TOKEN, ID_ERR);

    // Load the case variable value
    Codage_Lex(case_var.NOM);
    GENERER2(LDA, TABLESYM[IND_DER_SYM_ACC].ADRESSE);
    GENERER1(LDV);

    Test_Symbole(OF_TOKEN, OF_TOKEN);

    // Process first case
    // Load the constant to compare against
    GENERER2(LDI, atoi(SYM_COUR.NOM));
    Test_Symbole(NUM_TOKEN, NUM_ERR);

    // Generate equality comparison
    GENERER1(EQL);

    // Generate branch if false (BZE) to next case
    GENERER1(BZE);
    int branch_loc = PC;

    Test_Symbole(DDOT_TOKEN, DDOT_ERR);
    INST();

    // Generate branch to end of case statement
    GENERER1(BRN);
    branch_ends[branch_count++] = PC;

    // Update the BZE to point to the next case
    PCODE[branch_loc].SUITE = PC + 1;

    // Handle subsequent case branches
    while (SYM_COUR.CODE == PV_TOKEN) {
        Sym_Suiv();  // Skip semicolon
        if (SYM_COUR.CODE == ELSE_TOKEN ||
            SYM_COUR.CODE == END_TOKEN) break;

        // Reload case variable for next comparison
        Codage_Lex(case_var.NOM);
        GENERER2(LDA, TABLESYM[IND_DER_SYM_ACC].ADRESSE);
        GENERER1(LDV);

        // Load the constant to compare against
        GENERER2(LDI, atoi(SYM_COUR.NOM));
        Test_Symbole(NUM_TOKEN, NUM_ERR);

        // Generate equality comparison
        GENERER1(EQL);

        // Generate branch if false to next case
        GENERER1(BZE);
        branch_loc = PC;

        Test_Symbole(DDOT_TOKEN, DDOT_ERR);
        INST();

        // Generate branch to end of case statement
        GENERER1(BRN);
        branch_ends[branch_count++] = PC;

        // Update the BZE to point to the next case
        PCODE[branch_loc].SUITE = PC + 1;
    }

    // Handle ELSE part if present
    if (SYM_COUR.CODE == ELSE_TOKEN) {
        Sym_Suiv();
        INST();
        if (SYM_COUR.CODE == PV_TOKEN) {
            Sym_Suiv();
        }
    }

    Test_Symbole(END_TOKEN, END_ERR);

    // Update all branch endpoints to point to the end of the case statement
    case_end = PC + 1;
    for (int i = 0; i < branch_count; i++) {
        PCODE[branch_ends[i]].SUITE = case_end;
    }

    free(branch_ends);
}

void SaveInstToFile(INSTRUCTION INST) {
    if (FICH_SORTIE == NULL) {
        printf("Erreur: Fichier de sortie non initialisé\n");
        return;
    }

    switch (INST.MNE) {
        case ADD:
            fprintf(FICH_SORTIE, "ADD\n");
            break;
        case SUB:
            fprintf(FICH_SORTIE, "SUB\n");
            break;
        case MUL:
            fprintf(FICH_SORTIE, "MUL\n");
            break;
        case DIV:
            fprintf(FICH_SORTIE, "DIV\n");
            break;
        case EQL:
            fprintf(FICH_SORTIE, "EQL\n");
            break;
        case NEQ:
            fprintf(FICH_SORTIE, "NEQ\n");
            break;
        case GTR:
            fprintf(FICH_SORTIE, "GTR\n");
            break;
        case LSS:
            fprintf(FICH_SORTIE, "LSS\n");
            break;
        case GEQ:
            fprintf(FICH_SORTIE, "GEQ\n");
            break;
        case LEQ:
            fprintf(FICH_SORTIE, "LEQ\n");
            break;
        case PRN:
            fprintf(FICH_SORTIE, "PRN\n");
            break;
        case INN:
            fprintf(FICH_SORTIE, "INN\n");
            break;
        case INT:
            fprintf(FICH_SORTIE, "INT\t%d\n", INST.SUITE);
            break;
        case LDI:
            fprintf(FICH_SORTIE, "LDI\t%d\n", INST.SUITE);
            break;
        case LDA:
            fprintf(FICH_SORTIE, "LDA\t%d\n", INST.SUITE);
            break;
        case LDV:
            fprintf(FICH_SORTIE, "LDV\n");
            break;
        case STO:
            fprintf(FICH_SORTIE, "STO\n");
            break;
        case BRN:
            fprintf(FICH_SORTIE, "BRN\t%d\n", INST.SUITE);
            break;
        case BZE:
            fprintf(FICH_SORTIE, "BZE\t%d\n", INST.SUITE);
            break;
        case HLT:
            fprintf(FICH_SORTIE, "HLT\n");
            break;
        // Add the missing procedure-related instructions
        case CALL:
            fprintf(FICH_SORTIE, "CALL\t%d\n", INST.SUITE);
            break;
        case INIT_PROC:
            fprintf(FICH_SORTIE, "INIT_PROC\n");
            break;
        case PARAM:
            fprintf(FICH_SORTIE, "PARAM\n");
            break;
        case PARAM_REF:
            fprintf(FICH_SORTIE, "PARAM_REF\n");
            break;
        case RETOUR:
            fprintf(FICH_SORTIE, "RETOUR\n");
            break;
        default:
            fprintf(stderr, "Instruction P-code inconnue: %d\n", INST.MNE);
            break;
    }
}

void PROCEDURE_PARAM_LIST() {
    Test_Symbole(PO_TOKEN, PO_ERR);  // Opening parenthesis

    int currentProcIndex = index_Mots - 1;
    int paramCount = 0;

    while (SYM_COUR.CODE != PF_TOKEN) {
        // Temporary storage for parameter names before type specification
        #define MAX_PARAMS_IN_GROUP 10
        char paramNames[MAX_PARAMS_IN_GROUP][MAX_NAME_LENGTH];
        int paramNameCount = 0;

        // Check for VAR parameter
        int isVarParam = (SYM_COUR.CODE == VAR_TOKEN);
        if (isVarParam) {
            Sym_Suiv();
        }

        // Collect parameter names
        while (1) {
            // Get parameter name
            sym = SYM_COUR;
            Test_Symbole(ID_TOKEN, ID_ERR);

            // Store parameter name
            strcpy(paramNames[paramNameCount++], sym.NOM);

            // If next symbol is comma, continue collecting names
            if (SYM_COUR.CODE == VIR_TOKEN) {
                Sym_Suiv();
            }
            // If next symbol is colon, we're done collecting names
            else if (SYM_COUR.CODE == DDOT_TOKEN) {
                break;
            }
            else {
                Erreur(VIR_ERR);  // Unexpected token
            }
        }

        // Expect colon
        Test_Symbole(DDOT_TOKEN, DDOT_ERR);

        // Handle parameter type
        CODES_LEX paramType = SYM_COUR.CODE;
        switch (paramType) {
            case INT_TOKEN:
            case REAL_TOKEN:
            case STRING_TOKEN:
                Sym_Suiv();
                break;
            default:
                Erreur(TYPE_ERR);
                return;
        }

        // Add each parameter to symbol table
        for (int i = 0; i < paramNameCount; i++) {
            if (paramCount >= MAX_PARAMS) {
                Erreur(PARAM_ERR);
                return;
            }

            // Store parameter name in procedure's parameter list
            strcpy(TABLESYM[currentProcIndex].parameters[paramCount].paramName, paramNames[i]);
            TABLESYM[currentProcIndex].parameters[paramCount].isReference = isVarParam;

            // Set type information
            switch (paramType) {
                case INT_TOKEN:
                    TABLESYM[currentProcIndex].parameters[paramCount].isReal = 0;
                    TABLESYM[currentProcIndex].parameters[paramCount].isString = 0;
                    break;
                case REAL_TOKEN:
                    TABLESYM[currentProcIndex].parameters[paramCount].isReal = 1;
                    TABLESYM[currentProcIndex].parameters[paramCount].isString = 0;
                    break;
                case STRING_TOKEN:
                    TABLESYM[currentProcIndex].parameters[paramCount].isReal = 0;
                    TABLESYM[currentProcIndex].parameters[paramCount].isString = 1;
                    break;
            }

            // Add parameter to symbol table
            strcpy(TABLESYM[index_Mots].NOM, paramNames[i]);
            TABLESYM[index_Mots].CLASSE = ID_TOKEN;
            TABLESYM[index_Mots].ADRESSE = ++OFFSET;

            // Set type information in symbol table
            switch (paramType) {
                case INT_TOKEN:
                    TABLESYM[index_Mots].isReal = 0;
                    TABLESYM[index_Mots].isString = 0;
                    break;
                case REAL_TOKEN:
                    TABLESYM[index_Mots].isReal = 1;
                    TABLESYM[index_Mots].isString = 0;
                    break;
                case STRING_TOKEN:
                    TABLESYM[index_Mots].isReal = 0;
                    TABLESYM[index_Mots].isString = 1;
                    break;
            }

            index_Mots++;
            paramCount++;
        }

        // Check for more parameter groups or end of parameter list
        if (SYM_COUR.CODE == PV_TOKEN) {
            Sym_Suiv();
        } else if (SYM_COUR.CODE != PF_TOKEN) {
            Erreur(PARAM_ERR);
        }
    }

    TABLESYM[currentProcIndex].paramCount = paramCount;
    Test_Symbole(PF_TOKEN, PF_ERR);
}

void PROCEDURE_DECL() {
    Test_Symbole(PROCEDURE_TOKEN, PROCEDURE_ERR);

    sym = SYM_COUR;
    Test_Symbole(ID_TOKEN, ID_ERR);

    // Store the procedure's entry point
    int procEntry = PC + 2;  // After the BRN instruction

    // Add procedure to symbol table
    int procedureIndex = index_Mots;
    strcpy(TABLESYM[procedureIndex].NOM, sym.NOM);
    TABLESYM[procedureIndex].CLASSE = PROCEDURE_TOKEN;
    TABLESYM[procedureIndex].isProcedure = 1;
    TABLESYM[procedureIndex].ADRESSE = procEntry;
    TABLESYM[procedureIndex].paramCount = 0;
    index_Mots++;

    // Optional parameter list
    if (SYM_COUR.CODE == PO_TOKEN) {
        Test_Symbole(PO_TOKEN, PO_ERR);  // Opening parenthesis

        while (SYM_COUR.CODE != PF_TOKEN) {
            // Check for VAR parameter
            int isVarParam = (SYM_COUR.CODE == VAR_TOKEN);
            if (isVarParam) {
                Sym_Suiv();
            }

            // Get parameter name
            sym = SYM_COUR;
            Test_Symbole(ID_TOKEN, ID_ERR);

            // Store parameter info
            if (TABLESYM[procedureIndex].paramCount < MAX_PARAMS) {
                strcpy(TABLESYM[procedureIndex].parameters[TABLESYM[procedureIndex].paramCount].paramName, sym.NOM);
                TABLESYM[procedureIndex].parameters[TABLESYM[procedureIndex].paramCount].isReference = isVarParam;
            } else {
                Erreur(PARAM_ERR);
            }

            Test_Symbole(DDOT_TOKEN, DDOT_ERR);

            // Handle parameter type and add to symbol table
            switch (SYM_COUR.CODE) {
                case INT_TOKEN: {
                    // Add parameter to symbol table
                    strcpy(TABLESYM[index_Mots].NOM, sym.NOM);
                    TABLESYM[index_Mots].CLASSE = ID_TOKEN;
                    TABLESYM[index_Mots].ADRESSE = ++OFFSET;
                    TABLESYM[index_Mots].isReal = 0;
                    TABLESYM[index_Mots].isString = 0;

                    // Update procedure parameter info
                    TABLESYM[procedureIndex].parameters[TABLESYM[procedureIndex].paramCount].isReal = 0;
                    TABLESYM[procedureIndex].parameters[TABLESYM[procedureIndex].paramCount].isString = 0;

                    index_Mots++;
                    TABLESYM[procedureIndex].paramCount++;
                    Sym_Suiv();
                    break;
                }
                case REAL_TOKEN: {
                    // Add parameter to symbol table
                    strcpy(TABLESYM[index_Mots].NOM, sym.NOM);
                    TABLESYM[index_Mots].CLASSE = ID_TOKEN;
                    TABLESYM[index_Mots].ADRESSE = ++OFFSET;
                    TABLESYM[index_Mots].isReal = 1;
                    TABLESYM[index_Mots].isString = 0;

                    // Update procedure parameter info
                    TABLESYM[procedureIndex].parameters[TABLESYM[procedureIndex].paramCount].isReal = 1;
                    TABLESYM[procedureIndex].parameters[TABLESYM[procedureIndex].paramCount].isString = 0;

                    index_Mots++;
                    TABLESYM[procedureIndex].paramCount++;
                    Sym_Suiv();
                    break;
                }
                case STRING_TOKEN: {
                    // Add parameter to symbol table
                    strcpy(TABLESYM[index_Mots].NOM, sym.NOM);
                    TABLESYM[index_Mots].CLASSE = ID_TOKEN;
                    TABLESYM[index_Mots].ADRESSE = ++OFFSET;
                    TABLESYM[index_Mots].isReal = 0;
                    TABLESYM[index_Mots].isString = 1;

                    // Update procedure parameter info
                    TABLESYM[procedureIndex].parameters[TABLESYM[procedureIndex].paramCount].isReal = 0;
                    TABLESYM[procedureIndex].parameters[TABLESYM[procedureIndex].paramCount].isString = 1;

                    index_Mots++;
                    TABLESYM[procedureIndex].paramCount++;
                    Sym_Suiv();
                    break;
                }
                default:
                    Erreur(TYPE_ERR);
                    return;
            }

            // Check for more parameters
            if (SYM_COUR.CODE == PV_TOKEN) {
                Sym_Suiv();
            } else if (SYM_COUR.CODE != PF_TOKEN) {
                Erreur(PARAM_ERR);
            }
        }

        Test_Symbole(PF_TOKEN, PF_ERR);
    }

    Test_Symbole(PV_TOKEN, PV_ERR);

    // Generate jump to skip procedure code
    GENERER2(BRN, PC + 2);  // We'll update this later
    int skipAddr = PC;

    // Generate procedure code
    BLOCK();
    GENERER1(RETOUR);

    // Update the branch address to skip procedure
    PCODE[skipAddr].SUITE = PC + 1;

    Test_Symbole(PV_TOKEN, PV_ERR);
}

void CALL_PROCEDURE(int procIndex) {
    printf("Calling procedure at index: %d\n", procIndex);
    printf("Procedure name: %s\n", TABLESYM[procIndex].NOM);
    printf("Expected parameters: %d\n", TABLESYM[procIndex].paramCount);

    // Verify procedure index is valid
    if (procIndex < 0 || procIndex >= index_Mots || !TABLESYM[procIndex].isProcedure) {
        Erreur(CALL_ERR);
        return;
    }

    Test_Symbole(PO_TOKEN, PO_ERR);
    int paramCount = 0;

    // Parameter processing loop
    while (SYM_COUR.CODE != PF_TOKEN) {
        if (paramCount >= TABLESYM[procIndex].paramCount) {
            Erreur(PARAM_ERR);
            return;
        }

        PARAM_INFO* currentParam = &TABLESYM[procIndex].parameters[paramCount];

        // Handle parameter based on passing mode
        if (currentParam->isReference) {
            // Reference parameter (VAR)
            sym = SYM_COUR;
            Test_Symbole(ID_TOKEN, ID_ERR);

            int varIndex = RechercherSym(sym.NOM);
            if (varIndex == -1) {
                Erreur(ND_ERR);
                return;
            }

            // Type checking for reference parameters
            if (currentParam->isReal != TABLESYM[varIndex].isReal ||
                currentParam->isString != TABLESYM[varIndex].isString) {
                Erreur(TYPE_ERR);
                return;
            }

            GENERER2(LDA, TABLESYM[varIndex].ADRESSE);
            GENERER1(PARAM_REF);
        } else {
            // Value parameter - create a local copy
            EXPR();
            GENERER1(PARAM);
        }

        paramCount++;

        if (SYM_COUR.CODE == VIR_TOKEN) {
            Sym_Suiv();
            if (SYM_COUR.CODE == PF_TOKEN) {
                // Extra comma at end of parameter list
                Erreur(PARAM_ERR);
                return;
            }
        } else if (SYM_COUR.CODE != PF_TOKEN && paramCount < TABLESYM[procIndex].paramCount) {
            // Missing comma between parameters
            Erreur(VIR_ERR);
            return;
        }
    }

    // Check if we got all required parameters
    if (paramCount < TABLESYM[procIndex].paramCount) {
        Erreur(PARAM_ERR);
        return;
    }

    Test_Symbole(PF_TOKEN, PF_ERR);

    // Generate procedure call
    GENERER2(CALL, TABLESYM[procIndex].ADRESSE);
}
void FUNCTION_PARAM_LIST() {
    Test_Symbole(PO_TOKEN, PO_ERR);  // Opening parenthesis

    int currentFuncIndex = index_Mots - 1;
    int paramCount = 0;

    // Check if parameter list is empty
    if (SYM_COUR.CODE == PF_TOKEN) {
        Test_Symbole(PF_TOKEN, PF_ERR);
    } else {
        while (1) {
            // Temporary storage for parameter names before type specification
            #define MAX_PARAMS_IN_GROUP 10
            char paramNames[MAX_PARAMS_IN_GROUP][MAX_NAME_LENGTH];
            int paramNameCount = 0;

            // Check for VAR parameter
            int isVarParam = (SYM_COUR.CODE == VAR_TOKEN);
            if (isVarParam) {
                Sym_Suiv();
            }

            // Collect parameter names
            while (1) {
                // Get parameter name
                sym = SYM_COUR;
                Test_Symbole(ID_TOKEN, ID_ERR);

                // Store parameter name
                strcpy(paramNames[paramNameCount++], sym.NOM);

                // If next symbol is comma, continue collecting names
                if (SYM_COUR.CODE == VIR_TOKEN) {
                    Sym_Suiv();
                }
                // If next symbol is colon, we're done collecting names
                else if (SYM_COUR.CODE == DDOT_TOKEN) {
                    break;
                }
                else {
                    Erreur(VIR_ERR);  // Unexpected token
                }
            }

            // Expect colon
            Test_Symbole(DDOT_TOKEN, DDOT_ERR);

            // Handle parameter type
            CODES_LEX paramType = SYM_COUR.CODE;
            switch (paramType) {
                case INT_TOKEN:
                case REAL_TOKEN:
                case STRING_TOKEN:
                    Sym_Suiv();
                    break;
                default:
                    Erreur(TYPE_ERR);
                    return;
            }

            // Add each parameter to symbol table
            for (int i = 0; i < paramNameCount; i++) {
                if (paramCount >= MAX_PARAMS) {
                    Erreur(PARAM_ERR);
                    return;
                }

                // Store parameter name in function's parameter list
                strcpy(TABLESYM[currentFuncIndex].parameters[paramCount].paramName, paramNames[i]);
                TABLESYM[currentFuncIndex].parameters[paramCount].isReference = isVarParam;

                // Set type information
                switch (paramType) {
                    case INT_TOKEN:
                        TABLESYM[currentFuncIndex].parameters[paramCount].isReal = 0;
                        TABLESYM[currentFuncIndex].parameters[paramCount].isString = 0;
                        break;
                    case REAL_TOKEN:
                        TABLESYM[currentFuncIndex].parameters[paramCount].isReal = 1;
                        TABLESYM[currentFuncIndex].parameters[paramCount].isString = 0;
                        break;
                    case STRING_TOKEN:
                        TABLESYM[currentFuncIndex].parameters[paramCount].isReal = 0;
                        TABLESYM[currentFuncIndex].parameters[paramCount].isString = 1;
                        break;
                }

                // Add parameter to symbol table
                strcpy(TABLESYM[index_Mots].NOM, paramNames[i]);
                TABLESYM[index_Mots].CLASSE = ID_TOKEN;
                TABLESYM[index_Mots].ADRESSE = ++OFFSET;

                // Set type information in symbol table
                switch (paramType) {
                    case INT_TOKEN:
                        TABLESYM[index_Mots].isReal = 0;
                        TABLESYM[index_Mots].isString = 0;
                        break;
                    case REAL_TOKEN:
                        TABLESYM[index_Mots].isReal = 1;
                        TABLESYM[index_Mots].isString = 0;
                        break;
                    case STRING_TOKEN:
                        TABLESYM[index_Mots].isReal = 0;
                        TABLESYM[index_Mots].isString = 1;
                        break;
                }

                index_Mots++;
                paramCount++;
            }

            // Check for more parameter groups or end of parameter list
            if (SYM_COUR.CODE == PV_TOKEN) {
                Sym_Suiv();
                continue;
            } else if (SYM_COUR.CODE == PF_TOKEN) {
                break;
            } else {
                Erreur(PARAM_ERR);
            }
        }

        TABLESYM[currentFuncIndex].paramCount = paramCount;
        Test_Symbole(PF_TOKEN, PF_ERR);
    }

    // Function return type
    Test_Symbole(DDOT_TOKEN, DDOT_ERR);
    switch (SYM_COUR.CODE) {
        case INT_TOKEN:
            TABLESYM[currentFuncIndex].returnType = INT_TOKEN;
            TABLESYM[currentFuncIndex].isReal = 0;
            break;
        case REAL_TOKEN:
            TABLESYM[currentFuncIndex].returnType = REAL_TOKEN;
            TABLESYM[currentFuncIndex].isReal = 1;
            break;
        case STRING_TOKEN:
            TABLESYM[currentFuncIndex].returnType = STRING_TOKEN;
            TABLESYM[currentFuncIndex].isString = 1;
            break;
        default:
            Erreur(TYPE_ERR);
            return;
    }
    Sym_Suiv();
}

void FUNCTION_DECL() {
    Test_Symbole(FUNCTION_TOKEN, FUNCTION_ERR);

    sym = SYM_COUR;
    Test_Symbole(ID_TOKEN, ID_ERR);

    // Store the function's entry point
    int funcEntry = PC + 2;  // After the BRN instruction

    // Add function to symbol table
    int functionIndex = index_Mots;
    strcpy(TABLESYM[functionIndex].NOM, sym.NOM);
    TABLESYM[functionIndex].CLASSE = FUNCTION_TOKEN;
    TABLESYM[functionIndex].isFunction = 1;
    TABLESYM[functionIndex].ADRESSE = funcEntry;
    TABLESYM[functionIndex].paramCount = 0;
    TABLESYM[functionIndex].hasReturnValue = 0;
    index_Mots++;

    // Optional parameter list and return type
    FUNCTION_PARAM_LIST();

    Test_Symbole(PV_TOKEN, PV_ERR);

    // Generate jump to skip function code
    GENERER2(BRN, PC + 2);  // We'll update this later
    int skipAddr = PC;

    // Generate function body
    BLOCK();

    // Ensure a return value was set
    if (!TABLESYM[functionIndex].hasReturnValue) {
        Erreur(RETURN_ERR);
    }

    GENERER1(RETOUR);

    // Update the branch address to skip function
    PCODE[skipAddr].SUITE = PC + 1;

    Test_Symbole(PV_TOKEN, PV_ERR);
}

void RETURN_STMT() {
    Test_Symbole(RETURN_TOKEN, RETURN_ERR);

    // Find the current function
    int currentFuncIndex = -1;
    for (int i = index_Mots - 1; i >= 0; i--) {
        if (TABLESYM[i].isFunction) {
            currentFuncIndex = i;
            break;
        }
    }

    if (currentFuncIndex == -1) {
        Erreur(RETURN_ERR);  // Return outside of function
        return;
    }

    // Parse expression based on function return type
    EXPR();

    // Mark that return value has been set
    TABLESYM[currentFuncIndex].hasReturnValue = 1;

    // Generate code to store return value
    GENERER1(STO);
}

void CALL_FUNCTION(int funcIndex) {
    printf("Calling function at index: %d\n", funcIndex);
    printf("Function name: %s\n", TABLESYM[funcIndex].NOM);
    printf("Expected parameters: %d\n", TABLESYM[funcIndex].paramCount);

    // Verify function index is valid
    if (funcIndex < 0 || funcIndex >= index_Mots || !TABLESYM[funcIndex].isFunction) {
        Erreur(CALL_ERR);
        return;
    }

    Test_Symbole(PO_TOKEN, PO_ERR);
    int paramCount = 0;

    // Parameter processing loop
    while (SYM_COUR.CODE != PF_TOKEN) {
        if (paramCount >= TABLESYM[funcIndex].paramCount) {
            Erreur(PARAM_ERR);
            return;
        }

        PARAM_INFO* currentParam = &TABLESYM[funcIndex].parameters[paramCount];

        // Handle parameter based on passing mode
        if (currentParam->isReference) {
            // Reference parameter (VAR)
            sym = SYM_COUR;
            Test_Symbole(ID_TOKEN, ID_ERR);

            int varIndex = RechercherSym(sym.NOM);
            if (varIndex == -1) {
                Erreur(ND_ERR);
                return;
            }

            // Type checking for reference parameters
            if (currentParam->isReal != TABLESYM[varIndex].isReal ||
                currentParam->isString != TABLESYM[varIndex].isString) {
                Erreur(TYPE_ERR);
                return;
            }

            GENERER2(LDA, TABLESYM[varIndex].ADRESSE);
            GENERER1(PARAM_REF);
        } else {
            // Value parameter
            if (SYM_COUR.CODE == ID_TOKEN) {
                // If it's an identifier, load its value
                sym = SYM_COUR;
                int varIndex = RechercherSym(sym.NOM);
                if (varIndex == -1) {
                    Erreur(ND_ERR);
                    return;
                }

                // Type checking for value parameters
                if (currentParam->isReal != TABLESYM[varIndex].isReal ||
                    currentParam->isString != TABLESYM[varIndex].isString) {
                    Erreur(TYPE_ERR);
                    return;
                }

                GENERER2(LDA, TABLESYM[varIndex].ADRESSE);
                GENERER1(LDV);
                GENERER1(PARAM);
                Sym_Suiv();
            } else {
                // Numeric or other expression
                EXPR();
                GENERER1(PARAM);
            }
        }

        paramCount++;

        if (SYM_COUR.CODE == VIR_TOKEN) {
            Sym_Suiv();
            if (SYM_COUR.CODE == PF_TOKEN) {
                // Extra comma at end of parameter list
                Erreur(PARAM_ERR);
                return;
            }
        } else if (SYM_COUR.CODE != PF_TOKEN && paramCount < TABLESYM[funcIndex].paramCount) {
            // Missing comma between parameters
            Erreur(VIR_ERR);
            return;
        }
    }

    // Check if we got all required parameters
    if (paramCount < TABLESYM[funcIndex].paramCount) {
        Erreur(PARAM_ERR);
        return;
    }

    Test_Symbole(PF_TOKEN, PF_ERR);

    // Generate function call
    GENERER2(CALL, TABLESYM[funcIndex].ADRESSE);

    // Load the return value
    GENERER2(LDA, TABLESYM[funcIndex].ADRESSE);
    GENERER1(LDV);
}

int main(){
    fichier = fopen("TestPascal.txt", "r");
    FICH_SORTIE = fopen("fichierSortie.txt","w+");
    if (fichier == NULL){
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }
    Lire_Car();
    Sym_Suiv();
    PROGRAM();
    printf("Program execution completed.\n");
    if (SYM_COUR.CODE == EOF_TOKEN){
        printf("BRAVO: le programme est correcte!!!\n");
        FICH_SORTIE = fopen("fichierSortie.txt", "w");
        if (FICH_SORTIE == NULL) {
            printf("Erreur lors de l'ouverture du fichier de sortie\n");
            fclose(fichier);
            return 1;
        }
        for(int i = 0;i<PC+1;i++){
            SaveInstToFile(PCODE[i]);
        }
        printf("PCODE généré avec succès!!!\n");
    }
    else{
        printf("PAS BRAVO: fin de programme erronée!!!!\n");
        printf("Current Token: %d\n", SYM_COUR.CODE);
        printf("Current Lexeme: %s\n", SYM_COUR.NOM);
        Sym_Suiv();
    }

    fclose(fichier);
    fclose(FICH_SORTIE);

    return 0;
}
