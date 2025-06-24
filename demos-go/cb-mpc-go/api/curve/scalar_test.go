package curve

import (
	"bytes"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

func TestScalarEqual(t *testing.T) {
	curve, err := NewSecp256k1()
	require.NoError(t, err)
	defer curve.Free()

	t.Run("equal_scalars", func(t *testing.T) {
		scalar1, err := curve.RandomScalar()
		require.NoError(t, err)

		// Create a copy with the same bytes
		scalar2 := &Scalar{Bytes: make([]byte, len(scalar1.Bytes))}
		copy(scalar2.Bytes, scalar1.Bytes)

		assert.True(t, scalar1.Equal(scalar2), "scalars with same bytes should be equal")
		assert.True(t, scalar2.Equal(scalar1), "equality should be symmetric")
	})

	t.Run("different_scalars", func(t *testing.T) {
		scalar1, err := curve.RandomScalar()
		require.NoError(t, err)

		scalar2, err := curve.RandomScalar()
		require.NoError(t, err)

		// Very unlikely that two random scalars are equal
		assert.False(t, scalar1.Equal(scalar2), "different random scalars should not be equal")
	})

	t.Run("nil_scalars", func(t *testing.T) {
		scalar, err := curve.RandomScalar()
		require.NoError(t, err)

		var nilScalar *Scalar

		assert.False(t, scalar.Equal(nilScalar), "scalar should not equal nil")
		assert.False(t, nilScalar.Equal(scalar), "nil should not equal scalar")
		assert.False(t, nilScalar.Equal(nilScalar), "nil should not equal nil")
	})

	t.Run("self_equality", func(t *testing.T) {
		scalar, err := curve.RandomScalar()
		require.NoError(t, err)

		assert.True(t, scalar.Equal(scalar), "scalar should equal itself")
	})
}

func TestScalarAddCommutativity(t *testing.T) {
	curve, err := NewSecp256k1()
	require.NoError(t, err)
	defer curve.Free()

	t.Run("a_plus_b_equals_b_plus_a", func(t *testing.T) {
		a, err := curve.RandomScalar()
		require.NoError(t, err)

		b, err := curve.RandomScalar()
		require.NoError(t, err)

		// Compute a + b
		aPlusB, err := a.Add(b)
		require.NoError(t, err)

		// Compute b + a
		bPlusA, err := b.Add(a)
		require.NoError(t, err)

		// They should be equal
		assert.True(t, aPlusB.Equal(bPlusA), "a+b should equal b+a (commutativity)")
	})

	t.Run("multiple_random_pairs", func(t *testing.T) {
		// Test commutativity with multiple random pairs to increase confidence
		for i := 0; i < 10; i++ {
			a, err := curve.RandomScalar()
			require.NoError(t, err)

			b, err := curve.RandomScalar()
			require.NoError(t, err)

			aPlusB, err := a.Add(b)
			require.NoError(t, err)

			bPlusA, err := b.Add(a)
			require.NoError(t, err)

			assert.True(t, aPlusB.Equal(bPlusA), "commutativity failed for pair %d", i)
		}
	})
}

func TestScalarAddErrorHandling(t *testing.T) {
	curve, err := NewSecp256k1()
	require.NoError(t, err)
	defer curve.Free()

	t.Run("nil_operands", func(t *testing.T) {
		scalar, err := curve.RandomScalar()
		require.NoError(t, err)

		var nilScalar *Scalar

		// Test scalar + nil
		result, err := scalar.Add(nilScalar)
		assert.Error(t, err)
		assert.Nil(t, result)
		assert.Contains(t, err.Error(), "nil scalar operand")

		// Test nil + scalar
		result, err = nilScalar.Add(scalar)
		assert.Error(t, err)
		assert.Nil(t, result)
		assert.Contains(t, err.Error(), "nil scalar operand")

		// Test nil + nil
		result, err = nilScalar.Add(nilScalar)
		assert.Error(t, err)
		assert.Nil(t, result)
		assert.Contains(t, err.Error(), "nil scalar operand")
	})
}

func TestScalarString(t *testing.T) {
	curve, err := NewSecp256k1()
	require.NoError(t, err)
	defer curve.Free()

	t.Run("valid_scalar", func(t *testing.T) {
		scalar, err := curve.RandomScalar()
		require.NoError(t, err)

		str := scalar.String()
		assert.Contains(t, str, "Scalar(")
		assert.Contains(t, str, ")")
		// Should contain hex representation of the bytes
		assert.Greater(t, len(str), len("Scalar()"))
	})

	t.Run("nil_scalar", func(t *testing.T) {
		var nilScalar *Scalar
		str := nilScalar.String()
		assert.Equal(t, "<nil scalar>", str)
	})
}

func TestScalarAddAssociativity(t *testing.T) {
	curve, err := NewSecp256k1()
	require.NoError(t, err)
	defer curve.Free()

	t.Run("a_plus_b_plus_c_associativity", func(t *testing.T) {
		a, err := curve.RandomScalar()
		require.NoError(t, err)

		b, err := curve.RandomScalar()
		require.NoError(t, err)

		c, err := curve.RandomScalar()
		require.NoError(t, err)

		// Compute (a + b) + c
		aPlusB, err := a.Add(b)
		require.NoError(t, err)

		aPlusBPlusC, err := aPlusB.Add(c)
		require.NoError(t, err)

		// Compute a + (b + c)
		bPlusC, err := b.Add(c)
		require.NoError(t, err)

		aPlusBPlusC2, err := a.Add(bPlusC)
		require.NoError(t, err)

		// They should be equal
		assert.True(t, aPlusBPlusC.Equal(aPlusBPlusC2), "(a+b)+c should equal a+(b+c) (associativity)")
	})
}

func TestScalarBytesConsistency(t *testing.T) {
	curve, err := NewSecp256k1()
	require.NoError(t, err)
	defer curve.Free()

	t.Run("bytes_length_consistency", func(t *testing.T) {
		orderBytes := curve.Order()

		for i := 0; i < 5; i++ {
			scalar, err := curve.RandomScalar()
			require.NoError(t, err)

			assert.Equal(t, len(orderBytes), len(scalar.Bytes),
				"scalar byte length should match curve order length")
		}
	})

	t.Run("equal_scalars_have_equal_bytes", func(t *testing.T) {
		scalar1, err := curve.RandomScalar()
		require.NoError(t, err)

		// Create scalar2 with same bytes
		scalar2 := &Scalar{Bytes: make([]byte, len(scalar1.Bytes))}
		copy(scalar2.Bytes, scalar1.Bytes)

		assert.True(t, scalar1.Equal(scalar2))
		assert.True(t, bytes.Equal(scalar1.Bytes, scalar2.Bytes))
	})
}

func TestNewScalarFromInt64(t *testing.T) {
	t.Run("positive_values", func(t *testing.T) {
		testCases := []int64{1, 42, 100, 1000, 65536}

		for _, value := range testCases {
			scalar := NewScalarFromInt64(value)
			assert.NotNil(t, scalar, "scalar should not be nil for value %d", value)
			assert.NotNil(t, scalar.Bytes, "scalar bytes should not be nil for value %d", value)
			assert.Greater(t, len(scalar.Bytes), 0, "scalar bytes should not be empty for value %d", value)
		}
	})

	t.Run("negative_values", func(t *testing.T) {
		testCases := []int64{-1, -42, -100, -1000}

		for _, value := range testCases {
			scalar := NewScalarFromInt64(value)
			assert.NotNil(t, scalar, "scalar should not be nil for value %d", value)
			assert.NotNil(t, scalar.Bytes, "scalar bytes should not be nil for value %d", value)
			assert.Greater(t, len(scalar.Bytes), 0, "scalar bytes should not be empty for value %d", value)
		}
	})

	t.Run("zero_value", func(t *testing.T) {
		scalar := NewScalarFromInt64(0)
		assert.NotNil(t, scalar)
		assert.Equal(t, 0, len(scalar.Bytes))
	})

	t.Run("equality_consistency", func(t *testing.T) {
		// Same values should produce equal scalars
		scalar1 := NewScalarFromInt64(42)
		scalar2 := NewScalarFromInt64(42)

		assert.True(t, scalar1.Equal(scalar2), "scalars from same int64 value should be equal")
		assert.True(t, bytes.Equal(scalar1.Bytes, scalar2.Bytes), "bytes should be identical for same int64 value")
	})

	t.Run("different_values_not_equal", func(t *testing.T) {
		scalar1 := NewScalarFromInt64(42)
		scalar2 := NewScalarFromInt64(43)

		assert.False(t, scalar1.Equal(scalar2), "scalars from different int64 values should not be equal")
	})

	t.Run("addition_with_int64_scalars", func(t *testing.T) {
		// Test that scalars created from int64 work with addition
		scalar1 := NewScalarFromInt64(10)
		scalar2 := NewScalarFromInt64(5)

		sum, err := scalar1.Add(scalar2)
		require.NoError(t, err)
		assert.NotNil(t, sum)

		// The sum should be different from both operands
		assert.False(t, sum.Equal(scalar1))
		assert.False(t, sum.Equal(scalar2))

		scalar3 := NewScalarFromInt64(15)
		assert.True(t, scalar3.Equal(sum))
	})

	t.Run("string_representation", func(t *testing.T) {
		scalar := NewScalarFromInt64(123)
		str := scalar.String()

		assert.Contains(t, str, "Scalar(")
		assert.Contains(t, str, ")")
		assert.Contains(t, str, "7b")
		assert.Greater(t, len(str), len("Scalar()"))
	})
}
