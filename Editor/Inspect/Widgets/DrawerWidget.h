/*
#pragma once

#include "Foundation/Inspect/Container.h"
#include "Editor/Controls/Drawer/Drawer.h"
#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {

        class DrawerWidget : public Widget
        {
        public:
            REFLECT_DECLARE_OBJECT( DrawerWidget, Widget );

            DrawerWidget( Inspect::Container* container = NULL );

            virtual void CreateWindow( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void DestroyWindow() HELIUM_OVERRIDE;

            virtual void Read() HELIUM_OVERRIDE {}
            virtual bool Write() HELIUM_OVERRIDE { return true; }

            Drawer* GetDrawer() const;

            void SetLabel( const tstring& label );
            void SetIcon( const tstring& icon );

        protected:
            void OnIconChanged( const Attribute< tstring >::ChangeArgs& args );
            void OnLabelChanged( const Attribute< tstring >::ChangeArgs& args );

            Inspect::Container* m_Container;
            wxPanel* m_Panel;
            Drawer* m_Drawer;
        };

    }
}
*/