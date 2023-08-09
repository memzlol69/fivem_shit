#include <Windows.h>
#include <cstdint>
#include <functional>
#include <vector>
#include "std_inc.h"

#include "event_core.h"
#include "registry.h"

struct resource_impl {
    char pad[0x40];
    fwEvent<std::vector<char>*> onBeforeLoadScript;
};

bool executed = false;
BOOL APIENTRY DllMain(uint64_t, DWORD reason_for_call) {
    if (reason_for_call != DLL_PROCESS_ATTACH)
        return false;

    auto base_resources = reinterpret_cast<uint64_t>(GetModuleHandleA("citizen-resources-core.dll"));
    auto on_init_instance = (base_resources + 0xAE560);
    auto resource_manager = *reinterpret_cast<uint64_t*>(base_resources + 0xAE6B0);

    (*(fwEvent<resource_impl*>*)(on_init_instance)).Connect([](resource_impl* resource) {
        resource->onBeforeLoadScript.Connect([](std::vector<char>* fileData) {
            if (!executed) {
                executed = true;
                std::string code_to_exec = "\nprint('hello world')";
                fileData->insert(fileData->end(), code_to_exec.begin(), code_to_exec.end());
            }
        });
    });

    return true;
}
