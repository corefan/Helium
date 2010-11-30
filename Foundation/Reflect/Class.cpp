#include "Class.h"
#include "Element.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Helium::Reflect;

Class::Class()
: m_Creator (NULL)
{

}

Class::~Class()
{

}

Class* Class::Create()
{
    return new Class();
}

ElementPtr Class::Clone(Element* element)
{
    if (!element)
    {
        return NULL;
    }

    ElementPtr clone = AssertCast<Element>( Registry::GetInstance()->CreateInstance( element->GetClass()) );

    element->PreSerialize();

    clone->PreDeserialize();

    Class::Copy( element, clone );

    clone->PostDeserialize();

    element->PostSerialize();

    return clone;
}