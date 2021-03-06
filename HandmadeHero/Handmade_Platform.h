#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//
// Compilers
//
#ifndef COMPILER_MSVC
#define COMPILER_MSVC 0
#endif

#ifndef COMPILER_LLVM
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
	//TODO: More compilers!!!
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
#endif

#if COMPILER_MSVC
#include <intrin.h>
#endif

//
// Types
//
#include <stdint.h>
#include <stddef.h>


	typedef int8_t int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;
	typedef int32 bool32;

	typedef uint8_t uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;

	typedef size_t memory_index;

	typedef float real32;
	typedef double real64;

	typedef struct thread_context
	{
		int placeHolder;
	} thread_context;

	//Services that the platform layer provides to the game
	//----------------------------------------------------------

#define internal_func static
#define local_persist static
#define global_var static

#define Pi32 3.14159265359f

#if HANDMADE_SLOW
#define Assert(expression) if(!(expression)) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif

#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)
#define Terabytes(value) (Gigabytes(value) * 1024)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

	inline uint32 SafeTruncateUint64(uint64 value)
	{
		Assert(value <= 0xFFFFFFFF);
		return (uint32)value;
	}



#if HANDMADE_INTERNAL
	typedef struct debug_read_file_result
	{
		uint32 contentsSize;
		void* contents;
	} debug_read_file_result;

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context* thread, char* fileName)
	typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context* thread, void* memory)
	typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context* thread, char* fileName, uint32 memorySize, void* memory)
	typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);
#endif

	//Services that the game provides to the platform layer
	//(This may expand in the future)
	//----------------------------------------------------------

	//FOUR THINGS - timing, input, bitmap to use, sound to use
	typedef struct game_offscreen_buffer
	{
		void *memory;
		int width;
		int height;
		int pitch;
		int bytesPerPixel;
	} game_offscreen_buffer;

	typedef struct game_soundOutput_buffer
	{
		int sampleCount, samplesPerSecond;
		int16* pSamples;
	} game_soundOutput_buffer;

	typedef struct game_button_state
	{
		int halfTransitionCount;
		bool32 endedDown;
	} game_button_state;

	typedef struct game_controller_input
	{
		bool32 isConnected;
		bool32 isAnalog;
		real32 stickAverageX;
		real32 stickAverageY;

		union
		{
			game_button_state buttons[12];
			struct
			{
				game_button_state moveUp;
				game_button_state moveDown;
				game_button_state moveLeft;
				game_button_state moveRight;

				game_button_state actionUp;
				game_button_state actionDown;
				game_button_state actionLeft;
				game_button_state actionRight;

				game_button_state leftShoulder;
				game_button_state rightShoulder;

				game_button_state back;
				game_button_state start;

				//Always have this as last button
				game_button_state terminator;
			};
		};
	} game_controller_input;

	typedef struct game_input
	{
		game_button_state mouseButtons[5];
		int32 mouseX, mouseY, mouseZ;

		real32 dtForFrame;

		game_controller_input controllers[5];
	} game_input;

	typedef struct game_memory
	{
		bool32 isInitialized;
		uint64 permanentStorageSize;
		void* pPermanentStorage; //REQUIRED to be cleared to zero at startup

		uint64 transientStorageSize;
		void* pTransientStorage; //REQUIRED to be cleared to zero at startup

		debug_platform_read_entire_file* DEBUG_PlatformReadEntireFile;
		debug_platform_free_file_memory* DEBUG_PlatformFreeFileMemory;
		debug_platform_write_entire_file* DEBUG_PlatformWriteEntireFile;
	} game_memory;

#define GAME_UPDATE_AND_RENDER(name) void name(thread_context* thread, game_memory* memory, game_input* input, game_offscreen_buffer* buffer)
	typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#define GAME_GET_SOUND_SAMPLES(name) void name(thread_context* thread, game_memory* memory, game_soundOutput_buffer *soundBuffer)
	typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);

	inline game_controller_input* GetController(game_input* input, int controllerIndex)
	{
		Assert(controllerIndex < ArrayCount(input->controllers));
		return &input->controllers[controllerIndex];
	}

#ifdef __cplusplus
}
#endif