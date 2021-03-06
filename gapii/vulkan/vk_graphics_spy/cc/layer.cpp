/*
 * Copyright (C) 2017 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dlfcn.h>

#include "vulkan/vulkan.h"

#if defined(__GNUC__)
#define VK_LAYER_EXPORT __attribute__((visibility("default")))
#else
#define VK_LAYER_EXPORT
#endif

#define PROC(name) \
  static PFN_##name fn = (PFN_##name)(getProcAddress()("gapid_" #name)); \
  if (fn != nullptr) \
    return fn

extern "C" {

typedef void* (*eglGetProcAddress)(char const * procname);

eglGetProcAddress getProcAddress() {
  static void* libegl = dlopen("libEGL.so", RTLD_NOW);
  static eglGetProcAddress pa = (eglGetProcAddress)dlsym(libegl, "eglGetProcAddress");
  return pa;
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
VkGraphicsSpyGetDeviceProcAddr(VkDevice dev, const char *funcName) {
  PROC(vkGetDeviceProcAddr)(dev, funcName);
  return nullptr;
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
VkGraphicsSpyGetInstanceProcAddr(VkInstance instance, const char *funcName) {
  PROC(vkGetInstanceProcAddr)(instance, funcName);
  return nullptr;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t *pCount,
                                   VkLayerProperties *pProperties) {
  PROC(vkEnumerateInstanceLayerProperties)(pCount, pProperties);
  *pCount = 0;
  return VK_SUCCESS;
}

// On Android this must also be defined, even if we have 0
// layers to expose.
VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,
                                       VkExtensionProperties *pProperties) {
  PROC(vkEnumerateInstanceExtensionProperties)(pLayerName, pCount, pProperties);
  *pCount = 0;
  return VK_SUCCESS;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice device, uint32_t *pCount, VkLayerProperties * pProperties) {
  PROC(vkEnumerateDeviceLayerProperties)(device, pCount, pProperties);
  *pCount = 0;
  return VK_SUCCESS;
}

// On android this must also be defined, even if we have 0
// layers to expose.
VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceExtensionProperties(VkPhysicalDevice device,
                                     const char *pLayerName, uint32_t *pCount,
                                     VkExtensionProperties *pProperties) {
  PROC(vkEnumerateDeviceExtensionProperties)(device, pLayerName, pCount,
                                                      pProperties);
  *pCount = 0;
  return VK_SUCCESS;
}
} // extern "C"
