/*
 * Color.h
 *
 *  Created on: 2012-5-13
 *      Author: gtxx3600
 */

#ifndef COLOR_H_
#define COLOR_H_

class Color {
public:

  const static Color kRed;
  const static Color kGreen;
  const static Color kBlue;
  const static Color kWhite;
  const static Color kBlack;

  Color();
  Color(double r, double g, double b);
  Color Multiply(double a) const;
  Color Modulate(const Color &c) const;
  Color Add(const Color &c) const;
  Color Sub(const Color &c) const;
  void Print() const;
  Color MyAdd(const Color &c) const;
  Color MyDiv(const Color &c) const;

  static Color kGray(float a);

  bool IsValid() const;
  bool IsBlack() const;
  bool IsTooLarge() const;

  double red, green, blue;
};

#endif /* COLOR_H_ */
