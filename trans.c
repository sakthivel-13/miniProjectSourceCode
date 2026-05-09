// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.ccc

#include <stdio.h>
#include <stdlib.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

// Added prototypes
void displayRecords(FILE *fPtr);
void totalBalance(FILE *fPtr);
void searchRecord(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;
    unsigned int choice;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1);
    }

    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        // ✅ Added cases
        case 6:
            displayRecords(cfPtr);
            break;

        case 7:
            totalBalance(cfPtr);
            break;

        case 8:
            searchRecord(cfPtr);
            break;

        default:
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr);
}

// create formatted text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    int result;
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    }
    else
    {
        rewind(readPtr);
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n",
                "Acct", "Last Name", "First Name", "Balance");

        while (!feof(readPtr))
        {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);

            if (result != 0 && client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                        client.acctNum,
                        client.lastName,
                        client.firstName,
                        client.balance);
            }
        }
        fclose(writePtr);
    }
}

// update record
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - 100): ");
    scanf("%d", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account #%d has no information.\n", account);
    }
    else
    {
        printf("%-6d%-16s%-11s%10.2f\n\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);

        printf("Enter charge (+) or payment (-): ");
        scanf("%lf", &transaction);

        client.balance += transaction;

        printf("%-6d%-16s%-11s%10.2f\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);

        fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// delete record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0};
    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");
    scanf("%d", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account %d does not exist.\n", accountNum);
    }
    else
    {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

// create record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    scanf("%d", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account #%d already exists.\n", client.acctNum);
    }
    else
    {
        printf("Enter lastname, firstname, balance:\n");
        scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

        client.acctNum = accountNum;

        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// menu
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - end program\n"
           "6 - display all accounts\n"
           "7 - display total balance\n"
           "8 - search account\n? ");

    scanf("%u", &menuChoice);
    return menuChoice;
}

// display all records
void displayRecords(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6d%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

//  total balance
void totalBalance(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    double total = 0;

    rewind(fPtr);

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            total += client.balance;
        }
    }

    printf("\nTotal Bank Balance: %.2f\n", total);
}

// search record
void searchRecord(FILE *fPtr)
{
    unsigned int acc;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account number to search: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\n%-6d%-16s%-11s%10.2f\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.balance);
    }
}