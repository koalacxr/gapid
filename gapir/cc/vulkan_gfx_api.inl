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

// Note this file is included inside vulkan_gfx_api.h:
//
// namespace gapir {
//
// class Vulkan : public Api {
// public:

typedef std::unordered_map<VkPhysicalDevice, VkInstance> VkPhysicalDeviceToVkInstance;
typedef std::unordered_map<VkDevice, VkPhysicalDevice> VkDeviceToVkPhysicalDevice;
typedef std::unordered_map<VkQueue, VkDevice> VkQueueToVkDevice;
typedef std::unordered_map<VkCommandBuffer, VkDevice> VkCommandBufferToVkDevice;
typedef struct {
    VkPhysicalDeviceToVkInstance VkPhysicalDevicesToVkInstances;
    VkDeviceToVkPhysicalDevice VkDevicesToVkPhysicalDevices;
    VkQueueToVkDevice VkQueuesToVkDevices;
    VkCommandBufferToVkDevice VkCommandBuffersToVkDevices;
} IndirectMaps;

IndirectMaps mIndirectMaps;

// Function for wrapping around the normal vkCreateInstance to inject
// virtual swapchain as an additional enabled layer.
bool replayCreateVkInstance(Stack* stack, bool pushReturn);

// Function for wrapping around the normal vkCreateDevice to null the
// pNext field in VkDeviceCreateInfo.
bool replayCreateVkDevice(Stack* stack, bool pushReturn);

// Builtin function for registering instance-level function pointers and
// binding all physical devices associated with the given instance.
// The instance is popped from the top of the stack.
bool replayRegisterVkInstance(Stack* stack);

// Builtin function for destroying instance-level function pointers.
// The instance is popped from the top of the stack.
bool replayUnregisterVkInstance(Stack* stack);

// Builtin function for creating device-level function pointers.
// From the top of the stack, pop three arguments sequentially:
// - pointer to the VkDeviceCreateInfo struct for this device,
// - the device,
// - the physical device.
bool replayRegisterVkDevice(Stack* stack);

// Builtin function for destroying device-level function pointers.
// The device is popped from the top of the stack.
bool replayUnregisterVkDevice(Stack* stack);

// Builtin function for linking command buffers to their device.
// From the top of the stack, pop three arguments sequentially:
// - ponter to a sequence of command buffers,
// - number of command buffers,
// - the device.
bool replayRegisterVkCommandBuffers(Stack* stack);

// Builtin function for discarding linking of command buffers.
// From the top of the stack, pop two arguments sequentially:
// - ponter to a sequence of command buffers,
// - number of command buffers.
bool replayUnregisterVkCommandBuffers(Stack* stack);

bool toggleVirtualSwapchainReturnAcquiredImage(Stack* stack);

bool replayGetFenceStatus(Stack* stack, bool pushReturn);

bool replayAllocateImageMemory(Stack* stack, bool pushReturn);
