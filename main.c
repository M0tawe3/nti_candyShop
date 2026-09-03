#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define CANDY_KINDS   6U
#define BASKET_MAX    8U
#define NAME_LEN     16U

typedef struct {
    char     name[NAME_LEN];
    uint16_t price;      /* piastres */
    uint16_t stock;      /* how many are left on the shelf */
    uint16_t sold;       /* how many we sold today         */
} Candy_t;

typedef struct {
    uint8_t  candyId;
    uint8_t  qty;
} Line_t;

static Candy_t shelf[CANDY_KINDS];
static Line_t  basket[BASKET_MAX];
static uint8_t basketLines;      /* how many slots of the basket are used */
static uint32_t cashDrawer;

static void     openShop(void);
static void     showShelf(void);
static void     addToBasket(void);
static void     removeFromBasket(void);
static uint32_t basketTotal(void);
static void     showBasket(void);
static void     checkout(void);
static void     giveChange(uint32_t change);
static uint8_t  bestSeller(void);
static void     dayReport(void);

int main(void){
    int choice;

    openShop();

    do {
        printf("\n===== Candy Shop =====\n");
        printf("1. Show shelf\n");
        printf("2. Add to basket\n");
        printf("3. Remove from basket\n");
        printf("4. Show basket\n");
        printf("5. Checkout\n");
        printf("6. Day report\n");
        printf("0. Exit\n");
        printf("Choose: ");

        if (scanf("%d", &choice) != 1) {
            printf("Please enter a number.\n");

            /* Clear invalid input */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {
            }

            continue;
        }

        switch (choice) {
        case 1:
            showShelf();
            break;

        case 2:
            addToBasket();
            break;

        case 3:
            removeFromBasket();
            break;

        case 4:
            showBasket();
            break;

        case 5:
            checkout();
            break;

        case 6:
            dayReport();
            break;

        case 0:
            printf("Goodbye!\n");
            break;

        default:
            printf("Invalid choice. Please choose 0-6.\n");
            break;
        }

    } while (choice != 0);

    return 0;
}

static void openShop(void){
    strcpy(shelf[0].name, "Chocolate");
    shelf[0].price = 50U;
    shelf[0].stock = 10U;
    shelf[0].sold = 0U;

    strcpy(shelf[1].name, "Lollipop");
    shelf[1].price = 25U;
    shelf[1].stock = 15U;
    shelf[1].sold = 0U;

    strcpy(shelf[2].name, "Gummy Bear");
    shelf[2].price = 75U;
    shelf[2].stock = 8U;
    shelf[2].sold = 0U;

    strcpy(shelf[3].name, "Toffee");
    shelf[3].price = 100U;
    shelf[3].stock = 6U;
    shelf[3].sold = 0U;

    strcpy(shelf[4].name, "Caramel");
    shelf[4].price = 150U;
    shelf[4].stock = 5U;
    shelf[4].sold = 0U;

    strcpy(shelf[5].name, "Jelly");
    shelf[5].price = 200U;
    shelf[5].stock = 4U;
    shelf[5].sold = 0U;

    basketLines = 0U;
    cashDrawer = 0U;
}

static void showShelf(void){
    printf("\n--- Shelf ---\n");
    printf("ID\tName\t\tPrice\tStock\n");

    for (uint8_t i = 0U; i < CANDY_KINDS; i++) {
        printf("%u\t%-16s\t%u\t",
               i,
               shelf[i].name,
               shelf[i].price);

        if (shelf[i].stock == 0U) {
            printf("SOLD OUT");
        } else {
            printf("%u", shelf[i].stock);
        }

        printf("\n");
    }
}

static void addToBasket(void){
    int candyId, qty;

    printf("Enter candy ID: ");

    if (scanf("%d", &candyId) != 1) {
        printf("Invalid input.\n");
        return;
    }
    if (candyId < 0 || candyId >= CANDY_KINDS) {
        printf("No such candy.\n");
        return;
    }
    printf("Enter quantity: ");

    if (scanf("%d", &qty) != 1 || qty <= 0) {
        printf("Invalid input.\n");
        return;
    }

    for (uint8_t i = 0U; i < basketLines; i++) {
        if (basket[i].candyId == (uint8_t)candyId) {
            if ((uint16_t)qty + basket[i].qty > shelf[candyId].stock || (uint16_t)qty + basket[i].qty > UINT8_MAX) {
                printf("Quantity too large\n");
                return;
            }
            basket[i].qty += (uint8_t)qty;
            printf("Added to existing basket line.\n");
            return;
        }
    }
    if (basketLines >= BASKET_MAX) {
        printf("Basket is full.\n");
        return;
    }

    basket[basketLines].candyId = (uint8_t)candyId;
    basket[basketLines].qty = (uint8_t)qty;
    basketLines++;

    printf("Candy added to basket.\n");
}

static void removeFromBasket(void){
    int line;

    if (basketLines == 0U) {
        printf("Basket is empty.\n");
        return;
    }

    showBasket();

    printf("Enter basket line to remove: ");

    if (scanf("%d", &line) != 1) {
        printf("Invalid input.\n");
        return;
    }

    if (line < 0 || line >= basketLines) {
        printf("No such basket line.\n");
        return;
    }

    for (uint8_t i = (uint8_t)line; i < basketLines - 1U; i++) {
        basket[i] = basket[i + 1U];
    }

    basketLines--;

    printf("Basket line removed.\n");
}

static uint32_t basketTotal(void){
    uint32_t total = 0U;

    for (uint8_t i = 0U; i < basketLines; i++) {
        uint8_t id = basket[i].candyId;

        total += (uint32_t)shelf[id].price * basket[i].qty;
    }

    return total;
}

static void showBasket(void){
    printf("\n--- Basket ---\n");

    if (basketLines == 0U) {
        printf("Basket is empty.\n");
        printf("Total: 0 piastres\n");
        return;
    }

    printf("Line\tName\t\tQty\tEach\tCost\n");

    for (uint8_t i = 0U; i < basketLines; i++) {
        uint8_t id = basket[i].candyId;
        uint32_t cost = (uint32_t)shelf[id].price * basket[i].qty;

        printf("%u\t%-16s\t%u\t%u\t%lu\n",
               i,
               shelf[id].name,
               basket[i].qty,
               shelf[id].price,
               (unsigned long)cost);
    }

    printf("Total: %lu piastres\n",
           (unsigned long)basketTotal());
}

static void checkout(void){
    uint32_t total;
    uint32_t payment;

    if (basketLines == 0U) {
        printf("Basket is empty.\n");
        return;
    }

    total = basketTotal();

    printf("Total: %lu piastres\n",(unsigned long)total);

    printf("Money handed over: ");

    if (scanf("%lu", (unsigned long *)&payment) != 1) {
        printf("Invalid input.\n");
        return;
    }

    if (payment < total) {
        printf("Not enough money. You need %lu more piastres.\n",(unsigned long)(total - payment));
        return;
    }

    for (uint8_t i = 0U; i < basketLines; i++) {
        uint8_t id = basket[i].candyId;

        shelf[id].stock -= basket[i].qty;
        shelf[id].sold += basket[i].qty;
    }

    cashDrawer += total;

    giveChange(payment - total);

    basketLines = 0U;

    printf("Purchase complete.\n");
}

static void giveChange(uint32_t change){
    uint32_t coins500;
    uint32_t coins200;
    uint32_t coins100;
    uint32_t coins50;
    uint32_t coins25;

    if (change == 0U) {
        printf("No change, thank you.\n");
        return;
    }

    coins500 = change / 500U;
    change %= 500U;

    coins200 = change / 200U;
    change %= 200U;

    coins100 = change / 100U;
    change %= 100U;

    coins50 = change / 50U;
    change %= 50U;

    coins25 = change / 25U;
    change %= 25U;

    printf("Change:\n");
    printf("500s: %lu\n", (unsigned long)coins500);
    printf("200s: %lu\n", (unsigned long)coins200);
    printf("100s: %lu\n", (unsigned long)coins100);
    printf("50s:  %lu\n", (unsigned long)coins50);
    printf("25s:  %lu\n", (unsigned long)coins25);
}

static uint8_t bestSeller(void){
    uint8_t best = 0U;

    for (uint8_t i = 1U; i < CANDY_KINDS; i++) {
        if (shelf[i].sold > shelf[best].sold) {
            best = i;
        }
    }

    return best;
}

static void dayReport(void){
    uint32_t totalSold = 0U;
    uint8_t best;

    for (uint8_t i = 0U; i < CANDY_KINDS; i++) {
        totalSold += shelf[i].sold;
    }

    best = bestSeller();

    printf("\n--- End of Day Report ---\n");
    printf("Money in drawer: %lu piastres\n",
           (unsigned long)cashDrawer);

    printf("Candies sold: %lu\n",
           (unsigned long)totalSold);

    printf("Best seller: %s (%u sold)\n",
           shelf[best].name,
           shelf[best].sold);

    printf("Sold out:\n");

    for (uint8_t i = 0U; i < CANDY_KINDS; i++) {
        if (shelf[i].stock == 0U) {
            printf("- %s\n", shelf[i].name);
        }
    }
}
