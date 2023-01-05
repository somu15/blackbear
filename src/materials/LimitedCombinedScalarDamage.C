//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LimitedCombinedScalarDamage.h"

registerMooseObject("BlackBearApp", LimitedCombinedScalarDamage);
registerMooseObject("BlackBearApp", ADLimitedCombinedScalarDamage);

template <bool is_ad>
InputParameters
LimitedCombinedScalarDamageTempl<is_ad>::validParams()
{
  InputParameters params = ScalarDamageBaseTempl<is_ad>::validParams();

  params.addClassDescription(
      "Scalar damage model which is computed as a function of multiple scalar damage models");

  params.addRequiredParam<std::vector<MaterialName>>("damage_models",
                                                     "Name of the damage models used to compute "
                                                     "the damage index");

  MooseEnum combination_type("Maximum Product", "Maximum");
  params.addParam<MooseEnum>(
      "combination_type", combination_type, "How the damage models are combined");
  params.addParam<Real>("max_damage", 1.0, "maximum allowed damage");

  return params;
}

template <bool is_ad>
LimitedCombinedScalarDamageTempl<is_ad>::LimitedCombinedScalarDamageTempl(
    const InputParameters & parameters)
  : ScalarDamageBaseTempl<is_ad>(parameters),
    _combination_type(
        this->template getParam<MooseEnum>("combination_type").template getEnum<CombinationType>()),
    _max_damage(this->template getParam<Real>("max_damage")),
    _damage_models_names(this->template getParam<std::vector<MaterialName>>("damage_models"))
{
}

template <bool is_ad>
void
LimitedCombinedScalarDamageTempl<is_ad>::initialSetup()
{
  for (unsigned int i = 0; i < _damage_models_names.size(); ++i)
  {
    ScalarDamageBaseTempl<is_ad> * model = dynamic_cast<ScalarDamageBaseTempl<is_ad> *>(
        &this->getMaterialByName(_damage_models_names[i]));

    if (model)
      _damage_models.push_back(model);
    else
      this->template paramError("damage_model",
                                "Damage Model " + _damage_models_names[i] +
                                    " is not compatible with LimitedCombinedScalarDamage");
  }
}

template <bool is_ad>
void
LimitedCombinedScalarDamageTempl<is_ad>::updateQpDamageIndex()
{
  switch (_combination_type)
  {
    case CombinationType::Maximum:
      _damage_index[_qp] = _damage_index_old[_qp];
      for (unsigned int i = 0; i < _damage_models.size(); ++i)
        _damage_index[_qp] = std::max(_damage_index[_qp], _damage_models[i]->getQpDamageIndex(_qp));
      break;
    case CombinationType::Product:
      _damage_index[_qp] = 1.0;
      for (unsigned int i = 0; i < _damage_models.size(); ++i)
        _damage_index[_qp] *= 1.0 - _damage_models[i]->getQpDamageIndex(_qp);
      _damage_index[_qp] = 1.0 - _damage_index[_qp];
      break;
  }

  _damage_index[_qp] =
      std::min(std::max(_damage_index_old[_qp], std::max(0.0, std::min(1.0, _damage_index[_qp]))),
               _max_damage);
}

// #include "LimitedCombinedScalarDamage.h"

// registerMooseObject("GrizzlyApp", LimitedCombinedScalarDamage);
// registerMooseObject("GrizzlyApp", ADLimitedCombinedScalarDamage);

// template <bool is_ad>
// InputParameters
// LimitedCombinedScalarDamageTempl<is_ad>::validParams()
// {
//   InputParameters params = CombinedScalarDamageTempl<is_ad>::validParams();

//   params.addClassDescription("Limit the maximum damage value");
//   params.addParam<Real>("max_damage", 1.0, "Value of the maximum damage index");
//   return params;
// }

// template <bool is_ad>
// LimitedCombinedScalarDamageTempl<is_ad>::LimitedCombinedScalarDamageTempl(
//     const InputParameters & parameters)
//   : CombinedScalarDamageTempl<is_ad>(parameters),
//     _max_damage(this->template getParam<Real>("max_damage"))
// {
// }

// template <bool is_ad>
// void
// LimitedCombinedScalarDamageTempl<is_ad>::updateQpDamageIndex()
// {
//   switch (_combination_type)
//   {
//     case CombinationType::Maximum:
//       _damage_index[_qp] = _damage_index_old[_qp];
//       for (unsigned int i = 0; i < _damage_models.size(); ++i)
//         _damage_index[_qp] = std::max(_damage_index[_qp],
//         _damage_models[i]->getQpDamageIndex(_qp));
//       break;
//     case CombinationType::Product:
//       _damage_index[_qp] = 1.0;
//       for (unsigned int i = 0; i < _damage_models.size(); ++i)
//         _damage_index[_qp] *= 1.0 - _damage_models[i]->getQpDamageIndex(_qp);
//       _damage_index[_qp] = 1.0 - _damage_index[_qp];
//       break;
//   }

//   _damage_index[_qp] = std::max(
//       _damage_index_old[_qp], std::max(0.0, std::min(1.0, _damage_index[_qp])), _max_damage);
// }
