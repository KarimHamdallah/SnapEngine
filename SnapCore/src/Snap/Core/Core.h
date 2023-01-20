#pragma once

#ifndef SNAP_PLATFORM_WINDOWS
     #error Snap Only Supports Windows!
#endif

#define BIT(x) (1 << x)

#define SNAP_BIND_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

namespace SnapEngine
{
    template<typename T>
    using SnapPtr = std::shared_ptr<T>;

    template<typename T>
    using SnapUniquePtr = std::unique_ptr<T>;


    template<typename T, typename... Args>
    SnapPtr<T> CreatSnapPtr(Args&&... args)
    {
        return (SnapPtr<T>)std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    SnapUniquePtr<T> CreatSnapUiquePtr(Args&&... args)
    {
        return (SnapUniquePtr<T>)std::make_unique<T>(std::forward<Args>(args)...);
    }
}