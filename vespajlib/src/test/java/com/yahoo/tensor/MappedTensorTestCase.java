// Copyright 2016 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
package com.yahoo.tensor;

import org.junit.Test;

import java.util.Set;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

/**
 * Basic tensor tests. Tensor operations are tested in EvaluationTestCase
 *
 * @author bratseth
 */
public class MappedTensorTestCase {

    @Test
    public void testStringForm() {
        assertEquals("{}", Tensor.from("{}").toString());
        assertEquals("{{d1:l1,d2:l1}:5.0,{d1:l1,d2:l2}:6.0}", Tensor.from("{ {d1:l1,d2:l1}: 5,   {d2:l2, d1:l1}:6.0} ").toString());
        assertEquals("{{d1:l1,d2:l1}:-5.3,{d1:l1,d2:l2}:0.0}", Tensor.from("{ {d1:l1,d2:l1}:-5.3, {d2:l2, d1:l1}:0}").toString());
    }

    @Test
    public void testParseError() {
        try {
            Tensor.from("--");
            fail("Expected parse error");
        }
        catch (IllegalArgumentException expected) {
            assertEquals("Excepted a number or a string starting by { or tensor(, got '--'", expected.getMessage());
        }
    }

    @Test
    public void testDimensions() {
        Set<String> dimensions1 = Tensor.from("{} ").type().dimensionNames();
        assertEquals(0, dimensions1.size());

        Set<String> dimensions2 = Tensor.from("{ {d1:l1, d2:l2}:5, {d1:l2, d2:l2}:6.0} ").type().dimensionNames();
        assertEquals(2, dimensions2.size());
        assertTrue(dimensions2.contains("d1"));
        assertTrue(dimensions2.contains("d2"));

        Set<String> dimensions3 = Tensor.from("{ {d1:l1, d2:l1, d3:l1}:5, {d1:l1, d2:l2, d3:l1}:6.0} ").type().dimensionNames();
        assertEquals(3, dimensions3.size());
        assertTrue(dimensions3.contains("d1"));
        assertTrue(dimensions3.contains("d2"));
        assertTrue(dimensions3.contains("d3"));
    }

}