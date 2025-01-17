//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ScalarDamageBase.h"
/**
 * Scalar damage model computed as the combination of multiple damage models
 */
template <bool is_ad>
class LimitedCombinedScalarDamageTempl : public ScalarDamageBaseTempl<is_ad>
{
public:
  static InputParameters validParams();

  LimitedCombinedScalarDamageTempl(const InputParameters & parameters);

  void initialSetup() override;

protected:
  virtual void updateQpDamageIndex() override;

  enum class CombinationType
  {
    Maximum,
    Product
  };

  /// Type of expansion
  const CombinationType _combination_type;
  const Real _max_damage;

  std::vector<MaterialName> _damage_models_names;

  std::vector<ScalarDamageBaseTempl<is_ad> *> _damage_models;

  using ScalarDamageBaseTempl<is_ad>::_qp;
  using ScalarDamageBaseTempl<is_ad>::_damage_index;
  using ScalarDamageBaseTempl<is_ad>::_damage_index_old;
};

typedef LimitedCombinedScalarDamageTempl<false> LimitedCombinedScalarDamage;
typedef LimitedCombinedScalarDamageTempl<true> ADLimitedCombinedScalarDamage;

// #pragma once

// #include "CombinedScalarDamage.h"
// /**
//  * Limite the maximum scalar damage models computed as the combination of multiple damage models
//  */
// template <bool is_ad>
// class LimitedCombinedScalarDamageTempl : public CombinedScalarDamageTempl<is_ad>
// {
// public:
//   static InputParameters validParams();
//   LimitedCombinedScalarDamageTempl(const InputParameters & parameters);

// protected:
//   virtual void updateQpDamageIndex() override;
//   const Real _max_damage;
// };

// typedef LimitedCombinedScalarDamageTempl<false> LimitedCombinedScalarDamage;
// typedef LimitedCombinedScalarDamageTempl<true> ADLimitedCombinedScalarDamage;
