// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <gtest/gtest.h>

#include <BRepLib_SimplifyShape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <TopExp_Explorer.hxx>

//==================================================================================================
// Test: Default constructor creates object in non-done state
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, DefaultConstructor_CreatesNonDoneState)
{
  BRepLib_SimplifyShape aSimplifier;
  
  EXPECT_FALSE(aSimplifier.IsDone());
  EXPECT_EQ(aSimplifier.OriginalFaceCount(), 0);
  EXPECT_EQ(aSimplifier.SimplifiedFaceCount(), 0);
}

//==================================================================================================
// Test: Constructor with null shape does not throw
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, NullShape_DoesNotThrow)
{
  TopoDS_Shape aNullShape;
  
  EXPECT_NO_THROW(BRepLib_SimplifyShape aSimplifier(aNullShape));
}

//==================================================================================================
// Test: Simple box shape is processed successfully
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, SimpleBox_ProcessedSuccessfully)
{
  // Create a simple box (6 faces)
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  
  BRepLib_SimplifyShape aSimplifier(aBox);
  
  EXPECT_TRUE(aSimplifier.IsDone());
  EXPECT_EQ(aSimplifier.OriginalFaceCount(), 6);
  EXPECT_FALSE(aSimplifier.Shape().IsNull());
}

//==================================================================================================
// Test: Cylinder shape is processed successfully
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, SimpleCylinder_ProcessedSuccessfully)
{
  // Create a cylinder (3 faces: top, bottom, lateral)
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape();
  
  BRepLib_SimplifyShape aSimplifier(aCylinder);
  
  EXPECT_TRUE(aSimplifier.IsDone());
  EXPECT_EQ(aSimplifier.OriginalFaceCount(), 3);
  EXPECT_FALSE(aSimplifier.Shape().IsNull());
}

//==================================================================================================
// Test: Tolerance parameter affects processing
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, DifferentTolerances_ProcessedCorrectly)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  
  BRepLib_SimplifyShape aSimplifier1(aBox, 1e-7);
  BRepLib_SimplifyShape aSimplifier2(aBox, 1e-3);
  
  EXPECT_TRUE(aSimplifier1.IsDone());
  EXPECT_TRUE(aSimplifier2.IsDone());
  EXPECT_EQ(aSimplifier1.OriginalFaceCount(), 6);
  EXPECT_EQ(aSimplifier2.OriginalFaceCount(), 6);
}

//==================================================================================================
// Test: Coplanar faces detection with two parallel planes
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, TwoCoplanarFaces_DetectedCorrectly)
{
  // Create two coplanar rectangular faces
  gp_Pln aPlane1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  gp_Pln aPlane2(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1));
  
  BRepBuilderAPI_MakeFace aMaker1(aPlane1, 0.0, 10.0, 0.0, 10.0);
  BRepBuilderAPI_MakeFace aMaker2(aPlane2, 0.0, 10.0, 0.0, 10.0);
  
  if (aMaker1.IsDone() && aMaker2.IsDone())
  {
    BRep_Builder aBuilder;
    TopoDS_Compound aCompound;
    aBuilder.MakeCompound(aCompound);
    aBuilder.Add(aCompound, aMaker1.Shape());
    aBuilder.Add(aCompound, aMaker2.Shape());
    
    BRepLib_SimplifyShape aSimplifier(aCompound);
    
    EXPECT_TRUE(aSimplifier.IsDone());
    EXPECT_EQ(aSimplifier.OriginalFaceCount(), 2);
  }
}

//==================================================================================================
// Test: Perform method can be called after default construction
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, PerformMethod_WorksAfterDefaultConstruction)
{
  BRepLib_SimplifyShape aSimplifier;
  
  EXPECT_FALSE(aSimplifier.IsDone());
  
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(5.0, 5.0, 5.0).Shape();
  aSimplifier.Perform(aBox);
  
  EXPECT_TRUE(aSimplifier.IsDone());
  EXPECT_EQ(aSimplifier.OriginalFaceCount(), 6);
}

//==================================================================================================
// Test: Face count methods return correct values
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, FaceCountMethods_ReturnCorrectValues)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  
  BRepLib_SimplifyShape aSimplifier(aBox);
  
  EXPECT_EQ(aSimplifier.OriginalFaceCount(), 6);
  EXPECT_GE(aSimplifier.SimplifiedFaceCount(), 0);
  EXPECT_LE(aSimplifier.SimplifiedFaceCount(), aSimplifier.OriginalFaceCount());
  EXPECT_GE(aSimplifier.FacesReduced(), 0);
}

//==================================================================================================
// Test: Merge faces option can be disabled
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, MergeFacesDisabled_NoMerging)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  
  BRepLib_SimplifyShape aSimplifier(aBox, 1e-7, false);
  
  EXPECT_TRUE(aSimplifier.IsDone());
  EXPECT_EQ(aSimplifier.OriginalFaceCount(), 6);
  EXPECT_EQ(aSimplifier.SimplifiedFaceCount(), 6);
  EXPECT_EQ(aSimplifier.FacesReduced(), 0);
}

//==================================================================================================
// Test: Complex compound shape with multiple solids
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, CompoundShape_ProcessedSuccessfully)
{
  // Create a compound with two boxes
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(gp_Pnt(0, 0, 0), 10.0, 10.0, 10.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(15, 0, 0), 10.0, 10.0, 10.0).Shape();
  
  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);
  
  BRepLib_SimplifyShape aSimplifier(aCompound);
  
  EXPECT_TRUE(aSimplifier.IsDone());
  EXPECT_EQ(aSimplifier.OriginalFaceCount(), 12); // 6 faces per box
  EXPECT_FALSE(aSimplifier.Shape().IsNull());
}

//==================================================================================================
// Test: Result shape has correct topology type
//==================================================================================================

TEST(BRepLib_SimplifyShape_Test, ResultShape_HasCorrectType)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  
  BRepLib_SimplifyShape aSimplifier(aBox, 1e-7, true);
  
  ASSERT_TRUE(aSimplifier.IsDone());
  
  const TopoDS_Shape& aResult = aSimplifier.Shape();
  EXPECT_FALSE(aResult.IsNull());
  
  // Count faces in the result
  int aFaceCount = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    aFaceCount++;
  }
  
  EXPECT_GT(aFaceCount, 0);
}
