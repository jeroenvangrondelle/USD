//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#ifndef PXR_USD_SDF_PY_LIST_OP_H
#define PXR_USD_SDF_PY_LIST_OP_H

#include "pxr/pxr.h"
#include "pxr/usd/sdf/listOp.h"
#include "pxr/base/arch/demangle.h"
#include "pxr/base/tf/hash.h"
#include "pxr/base/tf/pyUtils.h"
#include "pxr/base/tf/stringUtils.h"
#include "pxr/external/boost/python.hpp"

PXR_NAMESPACE_OPEN_SCOPE

/// \class SdfPyWrapListOp
///
/// Helper class for wrapping SdfListOp objects for Python. The template
/// parameter is the specific SdfListOp type being wrapped (e.g.,
/// SdfPathListOp)
///
template <class T>
class SdfPyWrapListOp {
public:
    typedef typename T::ItemType   ItemType;
    typedef typename T::ItemVector ItemVector;

    typedef SdfPyWrapListOp<T> This;

    SdfPyWrapListOp(const std::string& name)
    {
        TfPyWrapOnce<T>([name]() { SdfPyWrapListOp::_Wrap(name); });
    }
 
private:
    static ItemVector _ApplyOperations1(const T& listOp, ItemVector input) {
        ItemVector result = input;
        listOp.ApplyOperations(&result);
        return result;
    }
    static pxr_boost::python::object
    _ApplyOperations2(const T& outer, const T& inner) {
        if (std::optional<T> r = outer.ApplyOperations(inner)) {
            return pxr_boost::python::object(*r);
        } else {
            return pxr_boost::python::object();
        }
    }

    static size_t _Hash(const T &self){
        return TfHash()(self);
    }

    static void _Wrap(const std::string& name)
    {
        using namespace pxr_boost::python;

        using ItemVector = typename T::ItemVector;

        class_<T>(name.c_str())
            .def("__str__", &This::_GetStr)
            .def("__hash__", &This::_Hash)

            .def("Create", &T::Create,
                 (arg("prependedItems") = ItemVector(),
                  arg("appendedItems") = ItemVector(),
                  arg("deletedItems") = ItemVector()))
            .staticmethod("Create")

            .def("CreateExplicit", &T::CreateExplicit,
                 (arg("explicitItems") = ItemVector()))
            .staticmethod("CreateExplicit")

            .def(self == self)
            .def(self != self)

            .def("HasItem", &T::HasItem)

            .def("Clear", &T::Clear)
            .def("ClearAndMakeExplicit", &T::ClearAndMakeExplicit)
            .def("ApplyOperations", &This::_ApplyOperations1)
            .def("ApplyOperations", &This::_ApplyOperations2)

            .add_property("explicitItems",
                make_function(&T::GetExplicitItems,
                              return_value_policy<return_by_value>()),
                +[](T& t, const ItemVector& items){t.SetExplicitItems(items);})
            .add_property("addedItems",
                make_function(&T::GetAddedItems,
                              return_value_policy<return_by_value>()),
                &T::SetAddedItems)
            .add_property("prependedItems",
                make_function(&T::GetPrependedItems,
                              return_value_policy<return_by_value>()),
                 +[](T& t, const ItemVector& items){t.SetPrependedItems(items);})
            .add_property("appendedItems",
                make_function(&T::GetAppendedItems,
                              return_value_policy<return_by_value>()),
                 +[](T& t, const ItemVector& items){t.SetAppendedItems(items);})
            .add_property("deletedItems",
                make_function(&T::GetDeletedItems,
                              return_value_policy<return_by_value>()),
                 +[](T& t, const ItemVector& items){t.SetDeletedItems(items);})
            .add_property("orderedItems",
                make_function(&T::GetOrderedItems,
                              return_value_policy<return_by_value>()),
                &T::SetOrderedItems)
            .def("GetAddedOrExplicitItems",
                &T::GetAppliedItems) //deprecated
            .def("GetAppliedItems",
                &T::GetAppliedItems)

            .add_property("isExplicit", &T::IsExplicit)

            ;

    }

    static std::string _GetStr(const T& listOp)
    {
        return TfStringify(listOp);
    }

};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_USD_SDF_PY_LIST_OP_H
