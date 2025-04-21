#include "MainController.h"
#include "AppService.h"

int main() {
    AppService appService;
    MainController controller(appService);
    controller.run();
    return 0;
}
