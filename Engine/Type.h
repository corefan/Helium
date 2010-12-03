//----------------------------------------------------------------------------------------------------------------------
// Type.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_TYPE_H
#define LUNAR_ENGINE_TYPE_H

#include "Foundation/Name.h"
#include "Foundation/Container/HashMap.h"
#include "Engine/GameObject.h"

namespace Lunar
{
    class Type;

    /// Reference counting support for Type types.
    class LUNAR_ENGINE_API TypeRefCountSupport
    {
    public:
        /// Base type of reference counted object.
        typedef Type BaseType;

        /// @name Reference Count Update Events
        //@{
        inline static void PreAddStrongRef( Type* pObject );
        inline static void PreRemoveStrongRef( Type* pObject );

        inline static void PreAddWeakRef( Type* pObject );
        inline static void PreRemoveWeakRef( Type* pObject );
        //@}

        /// @name Object Destruction Support
        //@{
        inline static void PreDestroy( Type* pObject );
        inline static void Destroy( Type* pObject );
        //@}

        /// @name Reference Count Proxy Allocation Interface
        //@{
        static RefCountProxy< Type >* Allocate();
        static void Release( RefCountProxy< Type >* pProxy );

        static void Shutdown();
        //@}

#if HELIUM_ENABLE_MEMORY_TRACKING
        /// @name Active Proxy Iteration
        //@{
        static size_t GetActiveProxyCount();
        static bool GetFirstActiveProxy( ConcurrentHashSet< RefCountProxy< Type >* >::ConstAccessor& rAccessor );
        //@}
#endif

    private:
        struct StaticData;

        /// Static proxy management data.
        static StaticData* sm_pStaticData;
    };

    /// Run-time type information for GameObject classes.
    class LUNAR_ENGINE_API Type : NonCopyable
    {
        HELIUM_DECLARE_REF_COUNT( Type, TypeRefCountSupport );

    public:
        /// Type lookup hash map.
        typedef HashMap< Name, TypePtr > LookupMap;

        /// General type flags.
        enum EFlag
        {
            /// Type should not be instantiated.
            FLAG_ABSTRACT    = 1 << 0,
            /// Instances of this type should never be saved or loaded in a package.
            FLAG_TRANSIENT   = 1 << 1,
            /// Instances of this type cannot be used as templates.
            FLAG_NO_TEMPLATE = 1 << 2
        };

        /// Type iterator.
        class LUNAR_ENGINE_API ConstIterator
        {
            friend class Type;

        public:
            /// @name Construction/Destruction
            //@{
            inline ConstIterator();
            //@}

            /// @name Overloaded Operators
            //@{
            inline Type& operator*() const;
            inline Type* operator->() const;

            inline ConstIterator& operator++();
            inline ConstIterator operator++( int );
            inline ConstIterator& operator--();
            inline ConstIterator operator--( int );

            inline bool operator==( const ConstIterator& rOther ) const;
            inline bool operator!=( const ConstIterator& rOther ) const;
            inline bool operator<( const ConstIterator& rOther ) const;
            inline bool operator>( const ConstIterator& rOther ) const;
            inline bool operator<=( const ConstIterator& rOther ) const;
            inline bool operator>=( const ConstIterator& rOther ) const;
            //@}

        private:
            /// Type map iterator.
            LookupMap::ConstIterator m_iterator;

            /// @name Construction/Destruction, Private
            //@{
            inline explicit ConstIterator( LookupMap::ConstIterator iterator );
            //@}
        };

        /// @name Construction/Destruction
        //@{
        Type();
        virtual ~Type();
        //@}

        /// @name Data Access
        //@{
        inline Name GetName() const;
        inline Type* GetTypeParent() const;
        inline GameObject* GetTypeTemplate() const;

        inline uint32_t GetTypeFlags() const;
        //@}

        /// @name Type Information
        //@{
        bool IsSubtypeOf( const Type* pType ) const;
        //@}

        /// @name Static Type Registration
        //@{
        inline static Package* GetTypePackage();
        static void SetTypePackage( Package* pPackage );

        static Type* Create( Name name, Package* pTypePackage, Type* pParent, GameObject* pTemplate, uint32_t flags );
        static void Unregister( Type* pType );

        static Type* Find( Name typeName );

        static ConstIterator GetTypeBegin();
        static ConstIterator GetTypeEnd();

        static void Shutdown();
        //@}

    private:
        /// Type name.
        Name m_name;
        /// Parent type.
        TypePtr m_spTypeParent;
        /// Default template object for this type.
        GameObjectPtr m_spTypeTemplate;

        /// Type flags.
        uint32_t m_typeFlags;

        /// Main package containing all template objects.
        static PackagePtr sm_spTypePackage;
        /// Type lookup hash map instance.
        static LookupMap* sm_pLookupMap;
    };
}

#include "Engine/Type.inl"

#endif  // LUNAR_ENGINE_TYPE_H