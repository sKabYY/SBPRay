/*
 * Material.cc
 *
 *  Created on: 2012-5-13
 *      Author: gtxx3600
 */

#include "materials/Material.h"
#include "base/Color.h"

Material::Material() {
  diffusion = Color::kBlack;
  reflection = Color::kBlack;
  refraction = Color::kBlack;
  emittance = Color::kBlack;
  refractive_index = 1;
}

Material::Material(Color _diffusion, Color _reflection,
    Color _refraction, Color _emittance)
  :diffusion(_diffusion), reflection(_reflection),
   refraction(_refraction), emittance(_emittance), refractive_index(1) {

}

Material::Material(Color _diffusion, Color _reflection,
    Color _refraction, float _refractive_index)
  :diffusion(_diffusion), reflection(_reflection),
   refraction(_refraction), emittance(Color::kBlack),
   refractive_index(_refractive_index) {

}

Material::Material(Color _diffusion, Color _reflection)
    :diffusion(_diffusion), reflection(_reflection),
     refraction(Color::kBlack), emittance(Color::kBlack),
     refractive_index(1) {
}

Material::Material(Material & m)
{
  diffusion = m.diffusion;
  reflection = m.reflection;
  refraction = m.refraction;
  emittance = m.emittance;
}

Material::~Material(){
}

bool Material::IsLightSource() const {
  return !emittance.IsBlack();
}

Color Material::get_emittance() const {
  return emittance;
}
