// #include <GLFW/glfw3.h>
// #include <vulkan/vulkan.h>
// #include "camera.h"
// #include "hittables/hittable_list.h"
// #include "hittables/sphere.h"
// #include "materials/material.h"
// #include "math/vec3.h"
// #include "utils.h"
//
// int main() {
//
//   hittable_list world;
//
//   auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
//   auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
//   auto material_left = make_shared<dielectric>(1.50);
//   auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
//   auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
//
//   world.add(
//       make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0,
//       material_ground));
//   world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5,
//   material_center)); world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0),
//   0.5, material_left)); world.add(make_shared<sphere>(point3(-1.0, 0.0,
//   -1.0), 0.4, material_bubble)); world.add(make_shared<sphere>(point3(1.0,
//   0.0, -1.0), 0.5, material_right));
//
//   camera cam;
//
//   cam.aspect_ratio = 16.0 / 9.0;
//   cam.image_width = 2560;
//   cam.samples_per_pixel = 100;
//   cam.max_depth = 50;
//
//   cam.vfov = 50;
//   cam.lookfrom = point3(-2, 2, 1);
//   cam.lookat = point3(0, 0, -1);
//   cam.vup = vec3(0, 1, 0);
//
//   cam.render(world);
//
// }
//
// #include "imgui/imgui.h"
// #include "imgui/imgui_impl_glfw.h"
// #include "imgui/imgui_impl_vulkan.h"
// #include <GLFW/glfw3.h>
// #include <stdio.h>  // printf, fprintf
// #include <stdlib.h> // abort
// #include <vector>
// #define GLFW_INCLUDE_NONE
// #define GLFW_INCLUDE_VULKAN
//
// // Volk headers
// #ifdef IMGUI_IMPL_VULKAN_USE_VOLK
// #define VOLK_IMPLEMENTATION
// #include <volk.h>
// #endif
//
// // [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010
// to
// // maximize ease of testing and compatibility with old VS compilers. To link
// // with VS2010-era libraries, VS2015+ requires linking with
// // legacy_stdio_definitions.lib, which we do using this pragma. Your own
// project
// // should not be affected, as you are likely to link with a newer binary of
// GLFW
// // that is adequate for your version of Visual Studio.
// #if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
//     !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
// #pragma comment(lib, "legacy_stdio_definitions")
// #endif
//
// // #define APP_USE_UNLIMITED_FRAME_RATE
// #ifdef _DEBUG
// #define APP_USE_VULKAN_DEBUG_REPORT
// static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
// #endif
//
// // Data
// static VkAllocationCallbacks *g_Allocator = nullptr;
// static VkInstance g_Instance = VK_NULL_HANDLE;
// static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
// static VkDevice g_Device = VK_NULL_HANDLE;
// static uint32_t g_QueueFamily = (uint32_t)-1;
// static VkQueue g_Queue = VK_NULL_HANDLE;
// static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
// static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
//
// static ImGui_ImplVulkanH_Window g_MainWindowData;
// static uint32_t g_MinImageCount = 2;
// static bool g_SwapChainRebuild = false;
//
// static void glfw_error_callback(int error, const char *description) {
//   fprintf(stderr, "GLFW Error %d: %s\n", error, description);
// }
// static void check_vk_result(VkResult err) {
//   if (err == VK_SUCCESS)
//     return;
//   fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
//   if (err < 0)
//     abort();
// }
//
// static VkImage g_PixelImage = VK_NULL_HANDLE;
// static VkDeviceMemory g_PixelImageMemory = VK_NULL_HANDLE;
// static VkImageView g_PixelImageView = VK_NULL_HANDLE;
// static VkSampler g_PixelSampler = VK_NULL_HANDLE;
// static VkBuffer g_PixelStagingBuffer = VK_NULL_HANDLE;
// static VkDeviceMemory g_PixelStagingMemory = VK_NULL_HANDLE;
// static void *g_PixelStagingMapped = nullptr;
// static VkCommandPool g_PixelCmdPool = VK_NULL_HANDLE;
// static VkCommandBuffer g_PixelCmdBuffer = VK_NULL_HANDLE;
// static VkDescriptorSet g_PixelTextureId =
//     VK_NULL_HANDLE; // usable directly as ImTextureID
// static int g_PixelWidth = 0;
// static int g_PixelHeight = 0;
//
// static uint32_t PixelTex_FindMemoryType(uint32_t type_filter,
//                                         VkMemoryPropertyFlags props) {
//   VkPhysicalDeviceMemoryProperties mem_props;
//   vkGetPhysicalDeviceMemoryProperties(g_PhysicalDevice, &mem_props);
//   for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++)
//     if ((type_filter & (1 << i)) &&
//         (mem_props.memoryTypes[i].propertyFlags & props) == props)
//       return i;
//   IM_ASSERT(0 && "No suitable Vulkan memory type for pixel texture");
//   return 0;
// }
//
// // Call ONCE, after ImGui_ImplVulkan_Init(), before the main loop starts.
// static void SetupPixelTexture(int width, int height) {
//   g_PixelWidth = width;
//   g_PixelHeight = height;
//   VkResult err;
//
//   // 1. The GPU image the pixels will live in
//   {
//     VkImageCreateInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//     info.imageType = VK_IMAGE_TYPE_2D;
//     info.format = VK_FORMAT_R8G8B8A8_UNORM;
//     info.extent = {(uint32_t)width, (uint32_t)height, 1};
//     info.mipLevels = 1;
//     info.arrayLayers = 1;
//     info.samples = VK_SAMPLE_COUNT_1_BIT;
//     info.tiling = VK_IMAGE_TILING_OPTIMAL;
//     info.usage = VK_IMAGE_USAGE_SAMPLED_BIT |
//     VK_IMAGE_USAGE_TRANSFER_DST_BIT; info.sharingMode =
//     VK_SHARING_MODE_EXCLUSIVE; info.initialLayout =
//     VK_IMAGE_LAYOUT_UNDEFINED; err = vkCreateImage(g_Device, &info,
//     g_Allocator, &g_PixelImage); check_vk_result(err);
//
//     VkMemoryRequirements req;
//     vkGetImageMemoryRequirements(g_Device, g_PixelImage, &req);
//     VkMemoryAllocateInfo alloc_info = {};
//     alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//     alloc_info.allocationSize = req.size;
//     alloc_info.memoryTypeIndex = PixelTex_FindMemoryType(
//         req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//     err = vkAllocateMemory(g_Device, &alloc_info, g_Allocator,
//                            &g_PixelImageMemory);
//     check_vk_result(err);
//     err = vkBindImageMemory(g_Device, g_PixelImage, g_PixelImageMemory, 0);
//     check_vk_result(err);
//   }
//
//   // 2. A view onto that image
//   {
//     VkImageViewCreateInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//     info.image = g_PixelImage;
//     info.viewType = VK_IMAGE_VIEW_TYPE_2D;
//     info.format = VK_FORMAT_R8G8B8A8_UNORM;
//     info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//     info.subresourceRange.levelCount = 1;
//     info.subresourceRange.layerCount = 1;
//     err = vkCreateImageView(g_Device, &info, g_Allocator, &g_PixelImageView);
//     check_vk_result(err);
//   }
//
//   // 3. A sampler. Current imgui_impl_vulkan.h no longer takes a sampler
//   //    in AddTexture(), but a compatibility overload that accepts (and
//   //    ignores) one is still shipped, so creating it keeps this code
//   //    working whether your imgui checkout predates the change or not.
//   {
//     VkSamplerCreateInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//     info.magFilter = VK_FILTER_LINEAR;
//     info.minFilter = VK_FILTER_LINEAR;
//     info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//     info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
//     info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
//     info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
//     info.minLod = -1000;
//     info.maxLod = 1000;
//     info.maxAnisotropy = 1.0f;
//     err = vkCreateSampler(g_Device, &info, g_Allocator, &g_PixelSampler);
//     check_vk_result(err);
//   }
//
//   // 4. Register the image with the ImGui Vulkan backend -> get a
//   //    VkDescriptorSet, which doubles as your ImTextureID.
//   g_PixelTextureId =
//       ImGui_ImplVulkan_AddTexture(g_PixelSampler, g_PixelImageView,
//                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//   // 5. A staging buffer: CPU writes pixels here, GPU copies them into the
//   image
//   {
//     VkDeviceSize size = (VkDeviceSize)width * height * 4;
//
//     VkBufferCreateInfo buf_info = {};
//     buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//     buf_info.size = size;
//     buf_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//     buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//     err =
//         vkCreateBuffer(g_Device, &buf_info, g_Allocator,
//         &g_PixelStagingBuffer);
//     check_vk_result(err);
//
//     VkMemoryRequirements req;
//     vkGetBufferMemoryRequirements(g_Device, g_PixelStagingBuffer, &req);
//     VkMemoryAllocateInfo alloc_info = {};
//     alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//     alloc_info.allocationSize = req.size;
//     alloc_info.memoryTypeIndex = PixelTex_FindMemoryType(
//         req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//     err = vkAllocateMemory(g_Device, &alloc_info, g_Allocator,
//                            &g_PixelStagingMemory);
//     check_vk_result(err);
//     err = vkBindBufferMemory(g_Device, g_PixelStagingBuffer,
//                              g_PixelStagingMemory, 0);
//     check_vk_result(err);
//
//     // Keep it mapped permanently so UpdatePixelTexture() is just a memcpy
//     err = vkMapMemory(g_Device, g_PixelStagingMemory, 0, size, 0,
//                       &g_PixelStagingMapped);
//     check_vk_result(err);
//   }
//
//   // 6. A small dedicated command pool/buffer for the per-frame upload
//   {
//     VkCommandPoolCreateInfo pool_info = {};
//     pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//     pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//     pool_info.queueFamilyIndex = g_QueueFamily;
//     err =
//         vkCreateCommandPool(g_Device, &pool_info, g_Allocator,
//         &g_PixelCmdPool);
//     check_vk_result(err);
//
//     VkCommandBufferAllocateInfo cb_info = {};
//     cb_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//     cb_info.commandPool = g_PixelCmdPool;
//     cb_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//     cb_info.commandBufferCount = 1;
//     err = vkAllocateCommandBuffers(g_Device, &cb_info, &g_PixelCmdBuffer);
//     check_vk_result(err);
//   }
// }
//
// // Call every frame, after filling `pixels` with width*height RGBA8 values,
// // e.g. pixels[y*width+x] = IM_COL32(r, g, b, 255).
// static void UpdatePixelTexture(const void *pixels) {
//   VkDeviceSize size = (VkDeviceSize)g_PixelWidth * g_PixelHeight * 4;
//   memcpy(g_PixelStagingMapped, pixels, (size_t)size);
//
//   vkResetCommandBuffer(g_PixelCmdBuffer, 0);
//   VkCommandBufferBeginInfo begin_info = {};
//   begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//   begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//   vkBeginCommandBuffer(g_PixelCmdBuffer, &begin_info);
//
//   // We overwrite the whole image every frame, so starting the barrier
//   // from VK_IMAGE_LAYOUT_UNDEFINED (instead of tracking the previous
//   // layout) is valid and keeps this simple.
//   VkImageMemoryBarrier to_dst = {};
//   to_dst.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//   to_dst.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//   to_dst.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//   to_dst.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//   to_dst.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//   to_dst.image = g_PixelImage;
//   to_dst.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
//   to_dst.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//   vkCmdPipelineBarrier(g_PixelCmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//                        VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
//                        nullptr, 1, &to_dst);
//
//   VkBufferImageCopy region = {};
//   region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
//   region.imageExtent = {(uint32_t)g_PixelWidth, (uint32_t)g_PixelHeight, 1};
//   vkCmdCopyBufferToImage(g_PixelCmdBuffer, g_PixelStagingBuffer,
//   g_PixelImage,
//                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//
//   VkImageMemoryBarrier to_read = to_dst;
//   to_read.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//   to_read.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//   to_read.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//   to_read.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//   vkCmdPipelineBarrier(g_PixelCmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
//                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
//                        0, nullptr, 1, &to_read);
//
//   vkEndCommandBuffer(g_PixelCmdBuffer);
//
//   VkSubmitInfo submit = {};
//   submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//   submit.commandBufferCount = 1;
//   submit.pCommandBuffers = &g_PixelCmdBuffer;
//   vkQueueSubmit(g_Queue, 1, &submit, VK_NULL_HANDLE);
//   vkQueueWaitIdle(
//       g_Queue); // simplest possible sync -- see perf note at bottom of file
// }
//
// // Call once, before ImGui_ImplVulkan_Shutdown() during cleanup.
// static void CleanupPixelTexture() {
//   ImGui_ImplVulkan_RemoveTexture(g_PixelTextureId);
//   vkDestroyCommandPool(g_Device, g_PixelCmdPool, g_Allocator);
//   vkUnmapMemory(g_Device, g_PixelStagingMemory);
//   vkDestroyBuffer(g_Device, g_PixelStagingBuffer, g_Allocator);
//   vkFreeMemory(g_Device, g_PixelStagingMemory, g_Allocator);
//   vkDestroySampler(g_Device, g_PixelSampler, g_Allocator);
//   vkDestroyImageView(g_Device, g_PixelImageView, g_Allocator);
//   vkDestroyImage(g_Device, g_PixelImage, g_Allocator);
//   vkFreeMemory(g_Device, g_PixelImageMemory, g_Allocator);
// }
// #ifdef APP_USE_VULKAN_DEBUG_REPORT
// static VKAPI_ATTR VkBool32 VKAPI_CALL
// debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT
// objectType,
//              uint64_t object, size_t location, int32_t messageCode,
//              const char *pLayerPrefix, const char *pMessage, void *pUserData)
//              {
//   (void)flags;
//   (void)object;
//   (void)location;
//   (void)messageCode;
//   (void)pUserData;
//   (void)pLayerPrefix; // Unused arguments
//   fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage:
//   %s\n\n",
//           objectType, pMessage);
//   return VK_FALSE;
// }
// #endif // APP_USE_VULKAN_DEBUG_REPORT
//
// static bool
// IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties,
//                      const char *extension) {
//   for (const VkExtensionProperties &p : properties)
//     if (strcmp(p.extensionName, extension) == 0)
//       return true;
//   return false;
// }
//
// static void SetupVulkan(ImVector<const char *> instance_extensions) {
//   VkResult err;
// #ifdef IMGUI_IMPL_VULKAN_USE_VOLK
//   volkInitialize();
// #endif
//
//   // Create Vulkan Instance
//   {
//     VkInstanceCreateInfo create_info = {};
//     create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//
//     // Enumerate available extensions
//     uint32_t properties_count;
//     ImVector<VkExtensionProperties> properties;
//     vkEnumerateInstanceExtensionProperties(nullptr, &properties_count,
//     nullptr); properties.resize(properties_count); err =
//     vkEnumerateInstanceExtensionProperties(nullptr, &properties_count,
//                                                  properties.Data);
//     check_vk_result(err);
//
//     // Enable required extensions
//     if (IsExtensionAvailable(
//             properties,
//             VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
//       instance_extensions.push_back(
//           VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
// #ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
//     if (IsExtensionAvailable(properties,
//                              VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
//                              {
//       instance_extensions.push_back(
//           VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
//       create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
//     }
// #endif
//
//     // Enabling validation layers
// #ifdef APP_USE_VULKAN_DEBUG_REPORT
//     const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
//     create_info.enabledLayerCount = 1;
//     create_info.ppEnabledLayerNames = layers;
//     instance_extensions.push_back("VK_EXT_debug_report");
// #endif
//
//     // Create Vulkan Instance
//     create_info.enabledExtensionCount = (uint32_t)instance_extensions.Size;
//     create_info.ppEnabledExtensionNames = instance_extensions.Data;
//     err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
//     check_vk_result(err);
// #ifdef IMGUI_IMPL_VULKAN_USE_VOLK
//     volkLoadInstance(g_Instance);
// #endif
//
//     // Setup the debug report callback
// #ifdef APP_USE_VULKAN_DEBUG_REPORT
//     auto f_vkCreateDebugReportCallbackEXT =
//         (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
//             g_Instance, "vkCreateDebugReportCallbackEXT");
//     IM_ASSERT(f_vkCreateDebugReportCallbackEXT != nullptr);
//     VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
//     debug_report_ci.sType =
//         VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
//     debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
//                             VK_DEBUG_REPORT_WARNING_BIT_EXT |
//                             VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
//     debug_report_ci.pfnCallback = debug_report;
//     debug_report_ci.pUserData = nullptr;
//     err = f_vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci,
//                                            g_Allocator, &g_DebugReport);
//     check_vk_result(err);
// #endif
//   }
//
//   // Select Physical Device (GPU)
//   g_PhysicalDevice = ImGui_ImplVulkanH_SelectPhysicalDevice(g_Instance);
//   IM_ASSERT(g_PhysicalDevice != VK_NULL_HANDLE);
//
//   // Select graphics queue family
//   g_QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(g_PhysicalDevice);
//   IM_ASSERT(g_QueueFamily != (uint32_t)-1);
//
//   // Create Logical Device (with 1 queue)
//   {
//     ImVector<const char *> device_extensions;
//     device_extensions.push_back("VK_KHR_swapchain");
//
//     // Enumerate physical device extension
//     uint32_t properties_count;
//     ImVector<VkExtensionProperties> properties;
//     vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr,
//                                          &properties_count, nullptr);
//     properties.resize(properties_count);
//     vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr,
//                                          &properties_count, properties.Data);
// #ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
//     if (IsExtensionAvailable(properties,
//                              VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
//       device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
// #endif
//
//     const float queue_priority[] = {1.0f};
//     VkDeviceQueueCreateInfo queue_info[1] = {};
//     queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//     queue_info[0].queueFamilyIndex = g_QueueFamily;
//     queue_info[0].queueCount = 1;
//     queue_info[0].pQueuePriorities = queue_priority;
//     VkDeviceCreateInfo create_info = {};
//     create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//     create_info.queueCreateInfoCount =
//         sizeof(queue_info) / sizeof(queue_info[0]);
//     create_info.pQueueCreateInfos = queue_info;
//     create_info.enabledExtensionCount = (uint32_t)device_extensions.Size;
//     create_info.ppEnabledExtensionNames = device_extensions.Data;
//     err =
//         vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator,
//         &g_Device);
//     check_vk_result(err);
//     vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
//   }
//
//   // Create Descriptor Pool
//   // If you wish to load e.g. additional textures you may need to alter pools
//   // sizes and maxSets.
//   {
//     VkDescriptorPoolSize pool_sizes[] = {
//         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
//          IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE},
//         {VK_DESCRIPTOR_TYPE_SAMPLER,
//          IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE},
//     };
//     VkDescriptorPoolCreateInfo pool_info = {};
//     pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//     pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//     pool_info.maxSets = 0;
//     for (VkDescriptorPoolSize &pool_size : pool_sizes)
//       pool_info.maxSets += pool_size.descriptorCount;
//     pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
//     pool_info.pPoolSizes = pool_sizes;
//     err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator,
//                                  &g_DescriptorPool);
//     check_vk_result(err);
//   }
// }
//
// // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers
// used
// // by the demo. Your real engine/app may not use them.
// static void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd,
//                               VkSurfaceKHR surface, int width, int height) {
//   // Check for WSI support
//   VkBool32 res;
//   vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily,
//   surface,
//                                        &res);
//   if (res != VK_TRUE) {
//     fprintf(stderr, "Error no WSI support on physical device 0\n");
//     exit(-1);
//   }
//
//   // Select Surface Format
//   const VkFormat requestSurfaceImageFormat[] = {
//       VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
//       VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
//   const VkColorSpaceKHR requestSurfaceColorSpace =
//       VK_COLORSPACE_SRGB_NONLINEAR_KHR;
//   wd->Surface = surface;
//   wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
//       g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat,
//       (size_t)IM_COUNTOF(requestSurfaceImageFormat),
//       requestSurfaceColorSpace);
//
//   // Select Present Mode
// #ifdef APP_USE_UNLIMITED_FRAME_RATE
//   VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR,
//                                       VK_PRESENT_MODE_IMMEDIATE_KHR,
//                                       VK_PRESENT_MODE_FIFO_KHR};
// #else
//   VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
// #endif
//   wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
//       g_PhysicalDevice, wd->Surface, &present_modes[0],
//       IM_COUNTOF(present_modes));
//   // printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);
//
//   // Create SwapChain, RenderPass, Framebuffer, etc.
//   IM_ASSERT(g_MinImageCount >= 2);
//   ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice,
//   g_Device,
//                                          wd, g_QueueFamily, g_Allocator,
//                                          width, height, g_MinImageCount, 0);
// }
//
// static void CleanupVulkan() {
//   vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
//
// #ifdef APP_USE_VULKAN_DEBUG_REPORT
//   // Remove the debug report callback
//   auto f_vkDestroyDebugReportCallbackEXT =
//       (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
//           g_Instance, "vkDestroyDebugReportCallbackEXT");
//   f_vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, g_Allocator);
// #endif // APP_USE_VULKAN_DEBUG_REPORT
//
//   vkDestroyDevice(g_Device, g_Allocator);
//   vkDestroyInstance(g_Instance, g_Allocator);
// }
//
// static void CleanupVulkanWindow(ImGui_ImplVulkanH_Window *wd) {
//   ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, wd, g_Allocator);
//   vkDestroySurfaceKHR(g_Instance, wd->Surface, g_Allocator);
// }
//
// static void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data)
// {
//   VkSemaphore image_acquired_semaphore =
//       wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
//   VkSemaphore render_complete_semaphore =
//       wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
//   VkResult err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX,
//                                        image_acquired_semaphore,
//                                        VK_NULL_HANDLE, &wd->FrameIndex);
//   if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
//     g_SwapChainRebuild = true;
//   if (err == VK_ERROR_OUT_OF_DATE_KHR)
//     return;
//   if (err != VK_SUBOPTIMAL_KHR)
//     check_vk_result(err);
//
//   ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
//   {
//     err = vkWaitForFences(
//         g_Device, 1, &fd->Fence, VK_TRUE,
//         UINT64_MAX); // wait indefinitely instead of periodically checking
//     check_vk_result(err);
//
//     err = vkResetFences(g_Device, 1, &fd->Fence);
//     check_vk_result(err);
//   }
//   {
//     err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
//     check_vk_result(err);
//     VkCommandBufferBeginInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//     info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//     err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
//     check_vk_result(err);
//   }
//   {
//     VkRenderPassBeginInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//     info.renderPass = wd->RenderPass;
//     info.framebuffer = fd->Framebuffer;
//     info.renderArea.extent.width = wd->Width;
//     info.renderArea.extent.height = wd->Height;
//     info.clearValueCount = 1;
//     info.pClearValues = &wd->ClearValue;
//     vkCmdBeginRenderPass(fd->CommandBuffer, &info,
//     VK_SUBPASS_CONTENTS_INLINE);
//   }
//
//   // Record dear imgui primitives into command buffer
//   ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);
//
//   // Submit command buffer
//   vkCmdEndRenderPass(fd->CommandBuffer);
//   {
//     VkPipelineStageFlags wait_stage =
//         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//     VkSubmitInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//     info.waitSemaphoreCount = 1;
//     info.pWaitSemaphores = &image_acquired_semaphore;
//     info.pWaitDstStageMask = &wait_stage;
//     info.commandBufferCount = 1;
//     info.pCommandBuffers = &fd->CommandBuffer;
//     info.signalSemaphoreCount = 1;
//     info.pSignalSemaphores = &render_complete_semaphore;
//
//     err = vkEndCommandBuffer(fd->CommandBuffer);
//     check_vk_result(err);
//     err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
//     check_vk_result(err);
//   }
// }
//
// static void FramePresent(ImGui_ImplVulkanH_Window *wd) {
//   if (g_SwapChainRebuild)
//     return;
//   VkSemaphore render_complete_semaphore =
//       wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
//   VkPresentInfoKHR info = {};
//   info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//   info.waitSemaphoreCount = 1;
//   info.pWaitSemaphores = &render_complete_semaphore;
//   info.swapchainCount = 1;
//   info.pSwapchains = &wd->Swapchain;
//   info.pImageIndices = &wd->FrameIndex;
//   VkResult err = vkQueuePresentKHR(g_Queue, &info);
//   if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
//     g_SwapChainRebuild = true;
//   if (err == VK_ERROR_OUT_OF_DATE_KHR)
//     return;
//   if (err != VK_SUBOPTIMAL_KHR)
//     check_vk_result(err);
//   wd->SemaphoreIndex =
//       (wd->SemaphoreIndex + 1) %
//       wd->SemaphoreCount; // Now we can use the next set of semaphores
// }
//
// // Main code
// int main(int, char **) {
//   glfwSetErrorCallback(glfw_error_callback);
//   if (!glfwInit())
//     return 1;
//
//   // Create window with Vulkan context
//   glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//   float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(
//       glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
//   GLFWwindow *window =
//       glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale),
//                        "Dear ImGui GLFW+Vulkan example", nullptr, nullptr);
//   if (!glfwVulkanSupported()) {
//     printf("GLFW: Vulkan Not Supported\n");
//     return 1;
//   }
//
//   ImVector<const char *> extensions;
//   uint32_t extensions_count = 0;
//   const char **glfw_extensions =
//       glfwGetRequiredInstanceExtensions(&extensions_count);
//   for (uint32_t i = 0; i < extensions_count; i++)
//     extensions.push_back(glfw_extensions[i]);
//   SetupVulkan(extensions);
//
//   // Create Window Surface
//   VkSurfaceKHR surface;
//   VkResult err =
//       glfwCreateWindowSurface(g_Instance, window, g_Allocator, &surface);
//   check_vk_result(err);
//
//   // Create Framebuffers
//   int w, h;
//   glfwGetFramebufferSize(window, &w, &h);
//   ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
//   SetupVulkanWindow(wd, surface, w, h);
//
//   // Setup Dear ImGui context
//   IMGUI_CHECKVERSION();
//   ImGui::CreateContext();
//   ImGuiIO &io = ImGui::GetIO();
//   (void)io;
//   io.ConfigFlags |=
//       ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
//   io.ConfigFlags |=
//       ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
//
//   // Setup Dear ImGui style
//   ImGui::StyleColorsDark();
//   // ImGui::StyleColorsLight();
//
//   // Setup scaling
//   ImGuiStyle &style = ImGui::GetStyle();
//   style.ScaleAllSizes(
//       main_scale); // Bake a fixed style scale. (until we have a solution for
//                    // dynamic style scaling, changing this requires resetting
//                    // Style + calling this again)
//   style.FontScaleDpi =
//       main_scale; // Set initial font scale. (in docking branch: using
//                   // io.ConfigDpiScaleFonts=true automatically overrides this
//                   // for every window depending on the current monitor)
//
//   // Setup Platform/Renderer backends
//   ImGui_ImplGlfw_InitForVulkan(window, true);
//   ImGui_ImplVulkan_InitInfo init_info = {};
//   // init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your
//   // value of VkApplicationInfo::apiVersion, otherwise will default to header
//   // version.
//   init_info.Instance = g_Instance;
//   init_info.PhysicalDevice = g_PhysicalDevice;
//   init_info.Device = g_Device;
//   init_info.QueueFamily = g_QueueFamily;
//   init_info.Queue = g_Queue;
//   init_info.PipelineCache = g_PipelineCache;
//   init_info.DescriptorPool = g_DescriptorPool;
//   init_info.MinImageCount = g_MinImageCount;
//   init_info.ImageCount = wd->ImageCount;
//   init_info.Allocator = g_Allocator;
//   init_info.PipelineInfoMain.RenderPass = wd->RenderPass;
//   init_info.PipelineInfoMain.Subpass = 0;
//   init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
//   init_info.CheckVkResultFn = check_vk_result;
//   ImGui_ImplVulkan_Init(&init_info);
//
//   SetupPixelTexture(1920, 1080); // TODO: This to the cam somehow
//
//   // Load Fonts
//   // - If fonts are not explicitly loaded, Dear ImGui will select an embedded
//   // font: either AddFontDefaultVector() or AddFontDefaultBitmap().
//   //   This selection is based on (style.FontSizeBase * style.FontScaleMain *
//   //   style.FontScaleDpi) reaching a small threshold.
//   // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to
//   select
//   // them.
//   // - If a file cannot be loaded, AddFont functions will return a nullptr.
//   // Please handle those errors in your code (e.g. use an assertion, display
//   an
//   // error and quit).
//   // - Read 'docs/FONTS.md' for more instructions and details.
//   // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use
//   FreeType
//   // for higher quality font rendering.
//   // - Remember that in C/C++ if you want to include a backslash \ in a
//   string
//   // literal you need to write a double backslash \\ !
//   // style.FontSizeBase = 20.0f;
//   // io.Fonts->AddFontDefaultVector();
//   // io.Fonts->AddFontDefaultBitmap();
//   // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
//   // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
//   // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
//   // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
//   // ImFont* font =
//   // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
//   // IM_ASSERT(font != nullptr);
//
//   // Our state
//   bool show_demo_window = true;
//   bool show_another_window = false;
//   ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//
//   // Main loop
//   while (!glfwWindowShouldClose(window)) {
//     // Poll and handle events (inputs, window resize, etc.)
//     // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
//     // tell if dear imgui wants to use your inputs.
//     // - When io.WantCaptureMouse is true, do not dispatch mouse input data
//     to
//     // your main application, or clear/overwrite your copy of the mouse data.
//     // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
//     // data to your main application, or clear/overwrite your copy of the
//     // keyboard data. Generally you may always pass all inputs to dear imgui,
//     // and hide them from your application based on those two flags.
//     glfwPollEvents();
//
//     // Resize swap chain?
//     int fb_width, fb_height;
//     glfwGetFramebufferSize(window, &fb_width, &fb_height);
//     if (fb_width > 0 && fb_height > 0 &&
//         (g_SwapChainRebuild || g_MainWindowData.Width != fb_width ||
//          g_MainWindowData.Height != fb_height)) {
//       ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
//       ImGui_ImplVulkanH_CreateOrResizeWindow(
//           g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily,
//           g_Allocator, fb_width, fb_height, g_MinImageCount, 0);
//       g_MainWindowData.FrameIndex = 0;
//       g_SwapChainRebuild = false;
//     }
//     if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
//       ImGui_ImplGlfw_Sleep(10);
//       continue;
//     }
//
//     // Start the Dear ImGui frame
//     ImGui_ImplVulkan_NewFrame();
//     ImGui_ImplGlfw_NewFrame();
//     ImGui::NewFrame();
//
//     {
//       static std::vector<uint32_t> pixels(1920 * 1080);
//       for (int y = 0; y < 1080; y++)
//         for (int x = 0; x < 1920; x++)
//           pixels[y * 1920 + x] =
//               IM_COL32((x * 255) / 256, (y * 255) / 256, 128, 255); // TODO:
//               Cam
//       UpdatePixelTexture(pixels.data());
//
//       ImGui::Begin("Pixels");
//       ImGui::Image((ImTextureID)(intptr_t)g_PixelTextureId,
//                    ImVec2((float)g_PixelWidth, (float)g_PixelHeight));
//       ImGui::End();
//     }
//     // 1. Show the big demo window (Most of the sample code is in
//     // ImGui::ShowDemoWindow()! You can browse its code to learn more about
//     Dear
//     // ImGui!).
//     if (show_demo_window)
//       ImGui::ShowDemoWindow(&show_demo_window);
//
//     // 2. Show a simple window that we create ourselves. We use a Begin/End
//     pair
//     // to create a named window.
//     {
//       static float f = 0.0f;
//       static int counter = 0;
//
//       ImGui::Begin("Hello, world!"); // Create a window called "Hello,
//       world!"
//                                      // and append into it.
//
//       ImGui::Text("This is some useful text."); // Display some text (you can
//                                                 // use a format strings too)
//       ImGui::Checkbox(
//           "Demo Window",
//           &show_demo_window); // Edit bools storing our window open/close
//           state
//       ImGui::Checkbox("Another Window", &show_another_window);
//
//       ImGui::SliderFloat("float", &f, 0.0f,
//                          1.0f); // Edit 1 float using a slider from 0.0f
//                          to 1.0f
//       ImGui::ColorEdit3(
//           "clear color",
//           (float *)&clear_color); // Edit 3 floats representing a color
//
//       if (ImGui::Button("Button")) // Buttons return true when clicked (most
//                                    // widgets return true when
//                                    edited/activated)
//         counter++;
//       ImGui::SameLine();
//       ImGui::Text("counter = %d", counter);
//
//       ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
//                   1000.0f / io.Framerate, io.Framerate);
//       ImGui::End();
//     }
//
//     // 3. Show another simple window.
//     if (show_another_window) {
//       ImGui::Begin(
//           "Another Window",
//           &show_another_window); // Pass a pointer to our bool variable (the
//                                  // window will have a closing button that
//                                  will
//                                  // clear the bool when clicked)
//       ImGui::Text("Hello from another window!");
//       if (ImGui::Button("Close Me"))
//         show_another_window = false;
//       ImGui::End();
//     }
//
//     // Rendering
//     ImGui::Render();
//     ImDrawData *draw_data = ImGui::GetDrawData();
//     const bool is_minimized =
//         (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <=
//         0.0f);
//     if (!is_minimized) {
//       wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
//       wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
//       wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
//       wd->ClearValue.color.float32[3] = clear_color.w;
//       FrameRender(wd, draw_data);
//       FramePresent(wd);
//     }
//   }
//
//   // Cleanup
//   err = vkDeviceWaitIdle(g_Device);
//   check_vk_result(err);
//   CleanupPixelTexture();
//   ImGui_ImplVulkan_Shutdown();
//   ImGui_ImplGlfw_Shutdown();
//   ImGui::DestroyContext();
//
//   CleanupVulkanWindow(&g_MainWindowData);
//   CleanupVulkan();
//
//   glfwDestroyWindow(window);
//   glfwTerminate();
//
//   return 0;
// }

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

const std::vector<char const *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  void initVulkan() {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createGraphicsPipeline();
  }

  void createGraphicsPipeline() {
    vk::raii::ShaderModule shaderModule =
        createShaderModule(readFile("shaders/slang.spv"));
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = shaderModule,
        .pName = "vertMain"};
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = shaderModule,
        .pName = "fragMain"};
    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                        fragShaderStageInfo};
  }

  [[nodiscard]] vk::raii::ShaderModule
  createShaderModule(const std::vector<char> &code) const {
    vk::ShaderModuleCreateInfo createInfo{
        .codeSize = code.size() * sizeof(char),
        .pCode = reinterpret_cast<const uint32_t *>(code.data())};
    vk::raii::ShaderModule shaderModule{m_device, createInfo};
    return shaderModule;
  }

  static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("failed to open file!");
    }

    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();

    return buffer;
  }

  void createInstance() {
    /* INIT VALIDATION LAYERS */
    std::vector<char const *> requiredLayers;
    if (enableValidationLayers) {
      requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    auto layerProperties = m_context.enumerateInstanceLayerProperties();
    auto unsupportedLayerIt = std::ranges::find_if(
        requiredLayers, [&layerProperties](auto const &requiredLayer) {
          return std::ranges::none_of(
              layerProperties, [requiredLayer](auto const &layerProperty) {
                return strcmp(layerProperty.layerName, requiredLayer) == 0;
              });
        });
    if (unsupportedLayerIt != requiredLayers.end()) {
      throw std::runtime_error("Required layer not supported: " +
                               std::string(*unsupportedLayerIt));
    }

    /* CHECK EXTENSIONS */
    auto requiredExtensions = getRequiredInstanceExtensions();
    auto extensionProperties = m_context.enumerateInstanceExtensionProperties();
    auto unsupportedExtensionIt = std::ranges::find_if(
        requiredExtensions,
        [&extensionProperties](auto const &requiredExtension) {
          return std::ranges::none_of(
              extensionProperties,
              [requiredExtension](auto const &extensionProperty) {
                return strcmp(extensionProperty.extensionName,
                              requiredExtension) == 0;
              });
        });
    if (unsupportedExtensionIt != requiredExtensions.end()) {
      throw std::runtime_error("Required extension not supported: " +
                               std::string(*unsupportedExtensionIt));
    }

    /* SET UP m_instance */
    constexpr vk::ApplicationInfo appInfo{
        .pApplicationName = "Helo vulkan",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = vk::ApiVersion14};

    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()};
    m_instance = vk::raii::Instance(m_context, createInfo);
  }

  std::vector<const char *> getRequiredInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return extensions;
  }

  bool isDeviceSuitable(vk::raii::PhysicalDevice const &physicalDevice) {
    // Check if the physicalDevice supports the Vulkan 1.3 API version
    bool supportsVulkan1_3 =
        physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

    // Check if any of the queue families support graphics operations
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    bool supportsGraphics =
        std::ranges::any_of(queueFamilies, [](auto const &qfp) {
          return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
        });

    // Check if all required physicalDevice extensions are available
    auto availableDeviceExtensions =
        physicalDevice.enumerateDeviceExtensionProperties();
    bool supportsAllRequiredExtensions = std::ranges::all_of(
        m_required_device_extension,
        [&availableDeviceExtensions](auto const &requiredDeviceExtension) {
          return std::ranges::any_of(
              availableDeviceExtensions,
              [requiredDeviceExtension](auto const &availableDeviceExtension) {
                return strcmp(availableDeviceExtension.extensionName,
                              requiredDeviceExtension) == 0;
              });
        });

    // Check if the physicalDevice supports the required features (shader draw
    // parameters, dynamic rendering and extended dynamic state)
    auto features = physicalDevice.template getFeatures2<
        vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsRequiredFeatures =
        features.template get<vk::PhysicalDeviceVulkan11Features>()
            .shaderDrawParameters &&
        features.template get<vk::PhysicalDeviceVulkan13Features>()
            .dynamicRendering &&
        features
            .template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
            .extendedDynamicState;

    // Return true if the physicalDevice meets all the criteria
    return supportsVulkan1_3 && supportsGraphics &&
           supportsAllRequiredExtensions && supportsRequiredFeatures;
  }

  void pickPhysicalDevice() {
    std::vector<vk::raii::PhysicalDevice> physicalDevices =
        m_instance.enumeratePhysicalDevices();
    auto const devIter =
        std::ranges::find_if(physicalDevices, [&](auto const &physicalDevice) {
          return isDeviceSuitable(physicalDevice);
        });
    if (devIter == physicalDevices.end()) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
    m_physical_device = *devIter;
  }

  void createLogicalDevice() {
    // find the index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        m_physical_device.getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports both
    // graphics and present
    uint32_t queueIndex = ~0;
    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size();
         qfpIndex++) {
      if ((queueFamilyProperties[qfpIndex].queueFlags &
           vk::QueueFlagBits::eGraphics) &&
          m_physical_device.getSurfaceSupportKHR(qfpIndex, *m_surface)) {
        // found a queue family that supports both graphics and present
        queueIndex = qfpIndex;
        break;
      }
    }
    if (queueIndex == ~0) {
      throw std::runtime_error(
          "Could not find a queue for graphics and present -> terminating");
    }

    // query for Vulkan 1.3 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan11Features,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain = {
            {}, // vk::PhysicalDeviceFeatures2
            {.shaderDrawParameters =
                 true},                 // vk::PhysicalDeviceVulkan11Features
            {.dynamicRendering = true}, // vk::PhysicalDeviceVulkan13Features
            {.extendedDynamicState =
                 true} // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
        };

    // create a Device
    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = queueIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority};
    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount =
            static_cast<uint32_t>(m_required_device_extension.size()),
        .ppEnabledExtensionNames = m_required_device_extension.data()};

    m_device = vk::raii::Device(m_physical_device, deviceCreateInfo);
    m_graphics_queue = vk::raii::Queue(m_device, queueIndex, 0);
  }

  vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    const auto formatIt =
        std::ranges::find_if(availableFormats, [](const auto &format) {
          return format.format == vk::Format::eB8G8R8A8Srgb &&
                 format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });
    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
  }

  vk::PresentModeKHR chooseSwapPresentMode(
      std::vector<vk::PresentModeKHR> const &availablePresentModes) {
    assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) {
      return presentMode == vk::PresentModeKHR::eFifo;
    }));
    return std::ranges::any_of(availablePresentModes,
                               [](const vk::PresentModeKHR value) {
                                 return vk::PresentModeKHR::eMailbox == value;
                               })
               ? vk::PresentModeKHR::eMailbox
               : vk::PresentModeKHR::eFifo;
  }

  vk::Extent2D
  chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    return {std::clamp<uint32_t>(width, capabilities.minImageExtent.width,
                                 capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height,
                                 capabilities.maxImageExtent.height)};
  }

  uint32_t chooseSwapMinImageCount(
      vk::SurfaceCapabilitiesKHR const &surfaceCapabilities) {
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) &&
        (surfaceCapabilities.maxImageCount < minImageCount)) {
      minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
  }

  void createSwapChain() {
    vk::SurfaceCapabilitiesKHR surfaceCapabilities =
        m_physical_device.getSurfaceCapabilitiesKHR(*m_surface);
    swapChainExtent = chooseSwapExtent(surfaceCapabilities);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

    std::vector<vk::SurfaceFormatKHR> availableFormats =
        m_physical_device.getSurfaceFormatsKHR(*m_surface);
    swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    std::vector<vk::PresentModeKHR> availablePresentModes =
        m_physical_device.getSurfacePresentModesKHR(*m_surface);
    vk::PresentModeKHR presentMode =
        chooseSwapPresentMode(availablePresentModes);

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = *m_surface,
        .minImageCount = minImageCount,
        .imageFormat = swapChainSurfaceFormat.format,
        .imageColorSpace = swapChainSurfaceFormat.colorSpace,
        .imageExtent = swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = presentMode,
        .clipped = true};

    swapChain = vk::raii::SwapchainKHR(m_device, swapChainCreateInfo);
    swapChainImages = swapChain.getImages();
  };

  void createImageViews() {
    assert(swapChainImageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = swapChainSurfaceFormat.format,
        .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(m_window)) {
      // checks events like closing the window
      glfwPollEvents();
    }
  }

  void cleanup() {
    glfwDestroyWindow(m_window);

    glfwTerminate();
  }

  void initWindow() {
    // Initilize the GLFW window

    // this is needed; Initilizes glfw
    glfwInit();

    // Optinons for the window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create the window and store a pointer to it in m_window
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Helo vulkan", nullptr, nullptr);
  }

  void createSurface() {
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(*m_instance, m_window, nullptr, &_surface) !=
        0) {
      throw std::runtime_error("failed to create window surface!");
    }
    m_surface = vk::raii::SurfaceKHR(m_instance, _surface);
  }
  GLFWwindow *m_window = nullptr;
  vk::raii::Context m_context;
  vk::raii::Instance m_instance{nullptr};
  vk::raii::PhysicalDevice m_physical_device{nullptr};
  std::vector<const char *> m_required_device_extension{
      vk::KHRSwapchainExtensionName};
  vk::raii::Device m_device{nullptr};
  vk::raii::Queue m_graphics_queue{nullptr};
  vk::raii::SurfaceKHR m_surface{nullptr};
  vk::raii::SwapchainKHR swapChain = nullptr;
  std::vector<vk::Image> swapChainImages;
  vk::SurfaceFormatKHR swapChainSurfaceFormat;
  vk::Extent2D swapChainExtent;
  std::vector<vk::raii::ImageView> swapChainImageViews;
};

int main() {
  try {
    HelloTriangleApplication app;
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
