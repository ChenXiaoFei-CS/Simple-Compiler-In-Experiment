/*********************************************************
*  ģ������    SemanticAnalyser
*  �ļ�����    SemanticOpTable.h
*  �����ļ���  ��
*  �� �ã�     ���C���������������͵�������
*  �� �ߣ�     �����
*  ʱ �䣺     2020.6.12
*  �� ����     Version 1.1
**********************************************************/
#ifndef _SEMANTICOPTABLE_H_
#define _SEMANTICOPTABLE_H_
#include "Base.h"
#include <iostream>

/*************************************************
* ��������  semanticTableOpCheck
* ���ã�    Ԥģ��C���Ե�����������֮�������
* ������    Op��           ������������뷽��[in]
*           type1          �������һ����������������[in]
*           type2:         ������ڶ�����������������[in]
*           castSemanType  �����ǿ��ת�͵���������[in]
* ����ֵ:   SenabticType
**************************************************/
static SemanticType semanticTableOpCheck(std::string Op,SemanticType type1,SemanticType type2,SemanticType castSemanType) {
	if (Op=="=M") {
		if (type1==SemanticType::INT && type2 == SemanticType::INT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::DOUBLE && type2 == SemanticType::DOUBLE) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::DOUBLE && type2 == SemanticType::INT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::CHAR && type2 == SemanticType::CHAR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::FLOAT && type2 == SemanticType::FLOAT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::LONG && type2 == SemanticType::LONG) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::SHORT && type2 == SemanticType::SHORT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::INT_PTR && type2 == SemanticType::INT_PTR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::DOUBLE_PTR && type2 == SemanticType::DOUBLE_PTR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::CHAR_PTR && type2 == SemanticType::CHAR_PTR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::SHORT_PTR && type2 == SemanticType::SHORT_PTR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::LONG_PTR && type2 == SemanticType::LONG_PTR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::FLOAT_PTR && type2 == SemanticType::FLOAT_PTR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::CHAR_PTR_PTR && type2 == SemanticType::CHAR_PTR_PTR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::BOOL && type2 == SemanticType::BOOL) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::BOOL_PTR && type2 == SemanticType::BOOL_PTR) {
			return SemanticType::BOOL;
		}
	}
	if (Op=="+=M" || Op=="-=M" || Op=="*=M" || Op=="/=M" || Op=="%=") {
		if (type1 == SemanticType::INT && type2 == SemanticType::INT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::DOUBLE && type2 == SemanticType::DOUBLE) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::INT && type2 == SemanticType::DOUBLE) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::CHAR && type2 == SemanticType::CHAR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::FLOAT && type2 == SemanticType::FLOAT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::LONG && type2 == SemanticType::LONG) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::SHORT && type2 == SemanticType::SHORT) {
			return SemanticType::BOOL;
		}
	}
	if (Op == "[M") {
		if (type1 == SemanticType::INT_PTR && type2 == SemanticType::INT) {
			return SemanticType::INT;
		}
		if (type1 == SemanticType::DOUBLE_PTR && type2 == SemanticType::INT) {
			return SemanticType::DOUBLE;
		}
		if (type1 == SemanticType::CHAR_PTR && type2 == SemanticType::INT) {
			return SemanticType::CHAR;
		}
		if (type1 == SemanticType::SHORT_PTR && type2 == SemanticType::INT) {
			return SemanticType::SHORT;
		}
		if (type1 == SemanticType::LONG_PTR && type2 == SemanticType::INT) {
			return SemanticType::LONG;
		}
		if (type1 == SemanticType::FLOAT_PTR && type2 == SemanticType::INT) {
			return SemanticType::FLOAT;
		}
		if (type1 == SemanticType::BOOL_PTR && type2 == SemanticType::INT) {
			return SemanticType::BOOL;
		}
	}
	if (Op == "+M") {
		if (type1 == SemanticType::INT && type2 == SemanticType::INT) {
			return SemanticType::INT;
		}
		if (type1 == SemanticType::INT && type2 == SemanticType::DOUBLE) {
			return SemanticType::DOUBLE;
		}
		if (type1 == SemanticType::DOUBLE && type2 == SemanticType::INT) {
			return SemanticType::DOUBLE;
		}
		if (type1 == SemanticType::DOUBLE && type2 == SemanticType::DOUBLE) {
			return SemanticType::DOUBLE;
		}
	}
	if (Op == "CastR") {
		return castSemanType;
	}
	if (Op==">M" || Op=="<M" || Op=="<=M" || Op==">=M") {
		if (type1==SemanticType::INT && type2==SemanticType::INT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::DOUBLE && type2 == SemanticType::DOUBLE) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::FLOAT && type2 == SemanticType::FLOAT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::CHAR && type2 == SemanticType::CHAR) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::INT && type2 == SemanticType::DOUBLE) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::DOUBLE && type2 == SemanticType::INT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::CHAR && type2 == SemanticType::INT) {
			return SemanticType::BOOL;
		}
		if (type1 == SemanticType::INT && type2 == SemanticType::CHAR) {
			return SemanticType::BOOL;
		}
	}
	if (Op=="++L" || Op=="++R" || Op=="--L" || Op=="--R") {
		if (type1==SemanticType::INT) {
			return SemanticType::INT;
		}
		if (type1 == SemanticType::INT_PTR) {
			return SemanticType::INT_PTR;
		}
		if (type1 == SemanticType::CHAR_PTR) {
			return SemanticType::CHAR_PTR;
		}
		if (type1 == SemanticType::DOUBLE_PTR) {
			return SemanticType::DOUBLE_PTR;
		}
		if (type1 == SemanticType::FLOAT_PTR) {
			return SemanticType::FLOAT_PTR;
		}
		if (type1 == SemanticType::LONG_PTR) {
			return SemanticType::LONG_PTR;
		}
		if (type1 == SemanticType::SHORT_PTR) {
			return SemanticType::SHORT_PTR;
		}
		if (type1 == SemanticType::CLASS_PTR) {
			return SemanticType::CLASS_PTR;
		}
		if (type1 == SemanticType::VOID_PTR) {
			return SemanticType::VOID_PTR;
		}
		if (type1 == SemanticType::BOOL_PTR) {
			return SemanticType::BOOL_PTR;
		}
		if (type1 == SemanticType::CHAR_PTR_PTR) {
			return SemanticType::CHAR_PTR_PTR;
		}
	}
	return SemanticType::NONE;
}
#endif