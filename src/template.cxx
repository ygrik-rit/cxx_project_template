module;
#include "version.hxx"

#include <algorithm>
#include <cassert>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
module engine;

#define VK_CHECK(func)                                                         \
    {                                                                          \
        const VkResult result = func;                                          \
        if (result != VK_SUCCESS)                                              \
        {                                                                      \
            std::cerr << "Error calling function " << #func << " at "          \
                      << __FILE__ << ":" << __LINE__ << ". Result is "         \
                      << result << std::endl;                                  \
            assert(false);                                                     \
        }                                                                      \
    }

namespace tlt::render
{
PhysicalDevice::PhysicalDevice(
    VkPhysicalDevice                device,
    VkSurfaceKHR                    surface,
    const std::vector<std::string>& requestedExtensions,
    bool                            printEnumerations)
    : physicalDevice_{ device }
{

    // Features
    vkGetPhysicalDeviceFeatures2(physicalDevice_, &features_);

    // Properties
    vkGetPhysicalDeviceProperties2(physicalDevice_, &properties_);

    // Get memory properties
    vkGetPhysicalDeviceMemoryProperties2(physicalDevice_, &memoryProperties_);

    // Enumerate queues
    {
        uint32_t queueFamilyCount{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties2(
            physicalDevice_, &queueFamilyCount, nullptr);
        queueFamilyProperties_.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties2(
            physicalDevice_, &queueFamilyCount, queueFamilyProperties_.data());
    }

    // Enumerate extensions
    {
        uint32_t propertyCount{ 0 };
        VK_CHECK(vkEnumerateDeviceExtensionProperties(
            physicalDevice_, nullptr, &propertyCount, nullptr));
        std::vector<VkExtensionProperties> properties(propertyCount);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(
            physicalDevice_, nullptr, &propertyCount, properties.data()));

        std::ranges::transform(properties,
                               std::back_inserter(extensions_),
                               [](const VkExtensionProperties& property)
                               { return std::string(property.extensionName); });

        enabledExtensions_ =
            util::filterExtensions(extensions_, requestedExtensions);
    }

    if (surface != VK_NULL_HANDLE)
    {
        enumerateSurfaceFormats(surface);
        enumerateSurfaceCapabilities(surface);
        enumeratePresentationModes(surface);
    }

    // Print device extensions for debugging
    if (printEnumerations)
    {
        std::cerr << properties_.properties.deviceName << " "
                  << properties_.properties.vendorID << " ("
                  << properties_.properties.deviceID << ") - ";
        const auto apiVersion = properties_.properties.apiVersion;
        std::cerr << "Vulkan " << VK_API_VERSION_MAJOR(apiVersion) << "."
                  << VK_API_VERSION_MINOR(apiVersion) << "."
                  << VK_API_VERSION_PATCH(apiVersion) << "."
                  << VK_API_VERSION_VARIANT(apiVersion) << ")" << std::endl;

        std::cerr << "Extensions: " << std::endl;
        for (const auto& extension : extensions_)
        {
            std::cerr << "\t" << extension << std::endl;
        }

        std::cerr << "Supported surface formats: " << std::endl;
        for (const auto format : surfaceFormats_)
        {
#ifdef _WIN32
            std::cerr << "\t" << string_VkFormat(format.format) << " : "
                      << string_VkColorSpaceKHR(format.colorSpace) << std::endl;
#else
            std::cerr << "\t" << format.format << " : " << format.colorSpace
                      << std::endl;
#endif
        }

        std::cerr << "Supported presentation modes: " << std::endl;
        for (const auto mode : presentModes_)
        {
#ifdef _WIN32
            std::cerr << "\t" << string_VkPresentModeKHR(mode) << std::endl;
#else
            std::cerr << "\t" << mode << std::endl;
#endif
        }
    }

    uint32_t propertyCount{ 0 };
    VK_CHECK(vkEnumerateDeviceExtensionProperties(
        physicalDevice_, nullptr, &propertyCount, nullptr));
    std::vector<VkExtensionProperties> properties(propertyCount);
    VK_CHECK(vkEnumerateDeviceExtensionProperties(
        physicalDevice_, nullptr, &propertyCount, properties.data()));

    uint32_t queueFamilyCount{ 0 };
    vkGetPhysicalDeviceQueueFamilyProperties2(
        physicalDevice_, &queueFamilyCount, nullptr);
    queueFamilyProperties_.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties2(
        physicalDevice_, &queueFamilyCount, queueFamilyProperties_.data());

    std::ranges::transform(properties,
                           std::back_inserter(extensions_),
                           [](const VkExtensionProperties& property)
                           { return std::string(property.extensionName); });

    enabledExtensions_ =
        util::filterExtensions(extensions_, requestedExtensions);
}

void PhysicalDevice::enumerateSurfaceFormats(VkSurfaceKHR surface)
{
    uint32_t formatCount{ 0 };
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice_, surface, &formatCount, nullptr);
    surfaceFormats_.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice_, surface, &formatCount, surfaceFormats_.data());
}

void PhysicalDevice::enumerateSurfaceCapabilities(VkSurfaceKHR surface)
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice_, surface, &surfaceCapabilities_);
}

void PhysicalDevice::enumeratePresentationModes(VkSurfaceKHR surface)
{
    uint32_t presentModeCount{ 0 };
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice_, surface, &presentModeCount, nullptr);

    presentModes_.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice_, surface, &presentModeCount, presentModes_.data());
}

render_impl::render_impl(SDL_Window* window)
{
    uint32_t instanceLayerCount{ 0 };
    VK_CHECK(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));
    std::vector<VkLayerProperties> layers(instanceLayerCount);
    VK_CHECK(
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, layers.data()));
    std::vector<std::string> availableLayers;

    std::ranges::transform(layers,
                           std::back_inserter(availableLayers),
                           [](const VkLayerProperties& properties)
                           { return properties.layerName; });

    uint32_t extensionsCount{ 0 };
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionsCount);
    vkEnumerateInstanceExtensionProperties(
        nullptr, &extensionsCount, extensionProperties.data());
    std::vector<std::string> availableExtensions;
    std::ranges::transform(extensionProperties,
                           std::back_inserter(availableExtensions),
                           [](const VkExtensionProperties& properties)
                           { return properties.extensionName; });

    const std::vector<std::string> requestedInstanceLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<std::string> requestedInstanceExtensions = {
#if defined(VK_KHR_win32_surface)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#if defined(VK_KHR_wayland_surface)
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#endif
#if defined(VK_EXT_debug_utils)
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
#if defined(VK_KHR_surface)
        VK_KHR_SURFACE_EXTENSION_NAME,
#endif
    };
    const auto enabledInstanceLayers =
        util::filterExtensions(availableLayers, requestedInstanceLayers);
    const auto enabledInstanceExtensions = util::filterExtensions(
        availableExtensions, requestedInstanceExtensions);

    std::vector<const char*> instanceExtensions(
        enabledInstanceExtensions.size());
    std::ranges::transform(enabledInstanceExtensions,
                           instanceExtensions.begin(),
                           std::mem_fn(&std::string::c_str));
    std::vector<const char*> instanceLayers(enabledInstanceLayers.size());
    std::ranges::transform(enabledInstanceLayers,
                           instanceLayers.begin(),
                           std::mem_fn(&std::string::c_str));

    const VkApplicationInfo applicationInfo_ = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "Template With Vulkan",
        .applicationVersion = VK_MAKE_VERSION(PROJECT_VERSION_MAJOR,
                                              PROJECT_VERSION_MINOR,
                                              PROJECT_VERSION_PATCH),
        .apiVersion         = VK_API_VERSION_1_4,
    };

    const VkInstanceCreateInfo instanceInfo = {
        .sType               = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo    = &applicationInfo_,
        .enabledLayerCount   = static_cast<uint32_t>(instanceLayers.size()),
        .ppEnabledLayerNames = instanceLayers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(instanceExtensions.size()),
        .ppEnabledExtensionNames = instanceExtensions.data(),
    };
    VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &instance_));

#if defined(TEMPLATE_LINUX) && defined(VK_KHR_wayland_surface)
    if (enabledInstanceExtensions.contains(
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME))
    {
        const auto props   = SDL_GetWindowProperties(window);
        const auto surface = SDL_GetPointerProperty(
            props, SDL_PROP_WINDOW_CREATE_WAYLAND_WL_SURFACE_POINTER, nullptr);
        const auto display = SDL_GetPointerProperty(
            props, SDL_PROP_GLOBAL_VIDEO_WAYLAND_WL_DISPLAY_POINTER, nullptr);

        if (surface != nullptr && display != nullptr)
        {
            const VkWaylandSurfaceCreateInfoKHR ci = {
                .sType   = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
                .display = static_cast<wl_display*>(display),
                .surface = static_cast<wl_surface*>(surface)
            };
            VK_CHECK(
                vkCreateWaylandSurfaceKHR(instance_, &ci, nullptr, &surface_));
        }
    }
#endif
}

std::unordered_set<std::string> util::filterExtensions(
    std::vector<std::string> availableExtensions,
    std::vector<std::string> requestedExtensions)
{
    std::ranges::sort(availableExtensions);
    std::ranges::sort(requestedExtensions);
    std::vector<std::string> result;
    std::ranges::set_intersection(
        availableExtensions, requestedExtensions, std::back_inserter(result));
    return { result.begin(), result.end() };
}

std::vector<PhysicalDevice> render_impl::enumeratePhysicalDevices(
    const std::vector<std::string>& requestedExtensions) const
{
    uint32_t deviceCount{ 0 };
    VK_CHECK(vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr));
    assert(deviceCount > 0 || "No Vulkan devices found");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK(
        vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data()));
    std::vector<PhysicalDevice> physicalDevices;
    for (const auto device : devices)
    {
        physicalDevices.emplace_back(
            device, surface_, requestedExtensions, true);
    }
    return physicalDevices;
}
} // namespace tlt::render

namespace tlt::window
{
window_impl::window_impl(std::string title, int w, int h)
    : title(std::move(title))
    , width(w)
    , height(h)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::cout << "Cant init SDL: " << SDL_GetError() << std::endl;
    }
}

bool window_impl::create() noexcept
{
    window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_VULKAN);
    if (window)
    {
        running = true;
        std::cout << "Window created" << std::endl;
    }
    else
    {
        std::cout << "Cant create window:" << SDL_GetError() << std::endl;
    }
    return running;
}

void window_impl::close() noexcept
{
    SDL_DestroyWindow(window);
    window  = nullptr;
    running = false;

    SDL_Quit();
}

void window_impl::poll_event() noexcept
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                close();
                break;
            case SDL_EVENT_KEY_DOWN:
                std::cout << tlt::input::keycode_to_name(
                    static_cast<tlt::input::keycode>(event.key.key));
                break;
            default:
                break;
        }
    }
}

void window_impl::render() noexcept {}

bool window_impl::is_running() const noexcept
{
    return running;
}
} // namespace tlt::window