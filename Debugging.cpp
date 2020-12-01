#include "Global.h"
#include <time.h>

void hs_log_file(const char *str...)
{
    va_list va;
    va_start(va, str);

    FILE* log = G_->logFile;
    if (log != nullptr)
    {
        vfprintf(log, str, va);
        fflush(G_->logFile);
    }
    va_end(va);
}

void copy_log(const char *oldName, const char *newName)
{
    FILE *f = fopen(oldName, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char str[fsize + 1];
    fread(str, 1, fsize, f);
    fclose(f);

    str[fsize] = 0;

    auto file = fopen(newName, "w");

    if (file != nullptr)
    {
        fputs(str, file);
        fclose(file);
    }
}

HOOK_STATIC(DebugHelper, CrashCatcher, (void *exception_pointers) -> int)
{
    int ret = super(exception_pointers);

    char ftlLogName[256];
    char hsLogName[256];

    time_t timer;
    time(&timer);
    auto localTime = localtime(&timer);

    strftime(ftlLogName, 256, "crashlogs/crash.%m.%d.%H.%M.log.txt", localTime);
    strftime(hsLogName, 256, "crashlogs/crash.%m.%d.%H.%M.hs_log.txt", localTime);

    copy_log("FTL.log", ftlLogName);
    copy_log("FTL_HS.log", hsLogName);

    return ret;
}

HOOK_METHOD(WorldManager, CreateChoiceBox, (LocationEvent *event) -> void)
{
    hs_log_file("Creating event: %s\n", event->eventName.c_str());

    super(event);
}

HOOK_METHOD(WorldManager, CreateShip, (ShipEvent* shipEvent, bool boss) -> CompleteShip*)
{

    auto ret = super(shipEvent, boss);

    hs_log_file("Creating ShipEvent: %s %s %s %s\n",
                 shipEvent->name.empty() ? ".." : shipEvent->name.c_str(),
                 shipEvent->blueprint.empty() ? ".." : shipEvent->blueprint.c_str(),
                 shipEvent->auto_blueprint.empty() ? ".." : shipEvent->auto_blueprint.c_str(),
                 ret && ret->shipManager->myBlueprint.blueprintName.empty() ?  ".." : ret->shipManager->myBlueprint.blueprintName.c_str());

    return ret;
}