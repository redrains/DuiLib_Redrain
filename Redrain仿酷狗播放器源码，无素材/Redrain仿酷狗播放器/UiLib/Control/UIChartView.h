#ifndef UIChart_h__
#define UIChart_h__

#pragma once

namespace UiLib
{
	class CChartViewUI;

	typedef enum LocationAlign
	{
		LOCATION_TOP,
		LOCATION_RIGHT,
		LOCATION_BOTTOM,
	}lalign;
	
	typedef enum ChartAlignment
	{
		CHART_HORIZONTAL,
		CHART_VERTICAL
	}calignment;


	class UILIB_API CChartItem
	{
	public:
		CChartItem(double _DataValue = NULL):m_iValue(_DataValue){};
		~CChartItem(void){};

	public:
		void SetDataValue(double _DataValue){m_iValue = _DataValue;};
		double GetDataValue(){return m_iValue;};

	private:
		double m_iValue;
	};

	class UILIB_API CChartSeries
	{
	public:
		CChartSeries(LPCTSTR _LegendText,LPCTSTR _SeriesColorA,LPCTSTR _SeriesColorB = NULL);
		CChartSeries(LPCTSTR _LegendText,DWORD _SeriesColorA,DWORD _SeriesColorB);
		~CChartSeries(void);

	public:
		void SetLegendText(LPCTSTR _LegendText);
		LPCTSTR GetLegendText();
		void SetSeriesColor(LPCTSTR _SeriesColorA,LPCTSTR _SeriesColorB);
		void SetSeriesColor(DWORD _SeriesColorA,DWORD _SeriesColorB);
		DWORD GetSeriesColorA();
		DWORD GetSeriesColorB();
		
		bool AddSeriesData(CChartItem* _ChartItem);
		bool AddSeriesData(double _DataValue);
		bool SetSeriesData(int _Index,CChartItem* _ChartItem);
		bool SetSeriesData(int _Index,double _DataValue);
		bool InsertSeriesData(int _Index,CChartItem* _ChartItem);
		bool InsertSeriesData(int _Index,double _DataValue);
		bool RemoveSeriesDataAt(int _Index);
		void RemoveSeriesDataAll();

		CChartItem* GetSeriesData(int _Index);
		int  GetDataCount();

		void SetVisible(bool _Visible);
		bool GetVisible();

		void SetChartViewObj(CChartViewUI* _pChartViewUI);
		void Invalidate();

	private:
		bool				m_bIsVisible;
		UINT				m_uSeriesFondId;
		DWORD				m_dSeriesColorA;
		DWORD				m_dSeriesColorB;
		CDuiString			m_sLegendText;
		CChartViewUI*		m_pChartView;
		TStdPtrArray<CChartItem*>	m_rSeriesData;
	};

	class UILIB_API CChartXYAxis
	{
	public:
		CChartXYAxis(void);
		~CChartXYAxis(void);

	public:
		void SetTickLimis(int _MinTick,int _MaxTick,UINT _StepTick);
		void SetMinTick(int _MinTick);
		void SetMaxTick(int _MaxTick);
		void SetStepTick(int _StepTick = 0);
		int  GetMinTick();
		int  GetMaxTick();
		UINT GetSetpTick();
		void ResetTick();

		void AddLabel(LPCTSTR _DataLabel);
		void AddCustomTick(int _TickValue);
		void SetVisibleCoustomTick(bool _Visible);
		bool GetVisibleCoustomTick();

		void SetXYAxisFontId(int _FontId);
		int  GetXYAxisFontId();
		void SetXYAxisLineSize(UINT _LineSize);
		UINT GetXYAxisLineSize();
		void SetLegendMaxWidth(UINT _LegendMaxWidth);
		UINT GetLegendMaxWidth();
		void SetLegendMaxHeight(UINT _LegendMaxHeight);
		UINT GetLegendMaxHeight();
		int  GetMaxGraphWidth();
		int  GetMaxGraphHeight();
		void SetMaxAxisLabelWidth(UINT _AxisLabelWidth);
		UINT GetMaxAxisLabelWidth();
		void SetMaxAxisLabelHeight(UINT _AxisLabelHeight);
		UINT GetMaxAxisLabelHeight();
		void SetMaxDataLabelWidth(UINT _DataLabelWidth);
		UINT GetMaxDataLabelWidth();
		void SetLegendLocation(lalign _LegendLocation);
		lalign GetLegendLocation();
		void SetXYAxisLineColor(DWORD _LineColor);
		void SetXYAxisLineColor(LPCTSTR _LineColor);
		DWORD GetXYAxisLineColor();
		void CalLegendSize(HDC hDC);
		void CalMaxAxisLabelWidth(HDC hDC,CChartSeries* _ChartSeries);
		void CalMaxDataLabelWidth(HDC hDC,CChartSeries* _ChartSeries);
		void CalAxisDataLimits(CChartSeries* _ChartSeries);
		void RemoveAllDataLabel();
		UINT GetGroupCount();
		UINT GetTickCount();
		DWORD AddColor(DWORD _SrcValue,DWORD _AddValue);
		DWORD CutColor(DWORD _SrcValue,DWORD _CurValue);
		void SetVisibleBaseLine(bool _Visible);
		bool GetVisibleBaseLine();

		void SetChartViewObj(CChartViewUI* _pChartViewUI);
		void Invalidate();
		void DrawAxis(HDC hDC, const RECT& rcPaint,RECT& rcItem);

	private:
		bool				m_bVisibelCustomTick;
		bool				m_bVisibleBaseLine;
		int					m_iMaxGraphWidth;
		int 				m_iMaxGraphHeight;
		int					m_iMinTick;
		int					m_iMaxTick;
		int					m_iMinTickEx;
		int					m_iMaxTickEx;
		UINT				m_uStepTick;
		UINT				m_uCountLabel;
		UINT				m_uMaxAxisLabelWidth;
		UINT				m_uMaxAxisLabelHeight;
		UINT				m_uMaxDataLabelWidth;
		UINT				m_uXYAxisFontId;
		UINT				m_uXYAxisLineSize;
		UINT				m_uLegendMaxWidth;
		UINT				m_uLegendMaxHeight;
		int					m_iAxissCale;
		int					m_iMinBaseLine;
		int					m_iMaxBaseLine;
		double				m_dDataScale;
		lalign				m_uLegendLocation;
		lalign				m_uLabelLocation;
		calignment			m_uAlignment;
		DWORD				m_dXYAxisLineColor;
		RECT				m_rcChartPadding;
		CDuiString			m_sLegendTitel;
		CChartViewUI*		m_pChartView;
		TStdValArray<int>	m_rDataAxis;
		TStdValArray<int>	m_rDataCustomAxis;
		TStdValArray<CDuiString,LPCTSTR>	m_rChartLabels;
	};

	class UILIB_API CChartViewUI : public CHorizontalLayoutUI
	{
	public:
		CChartViewUI(void);
		~CChartViewUI(void);

		LPCTSTR GetClass() const;
		LPVOID  GetInterface(LPCTSTR pstrName);
		void	DoPaint(HDC hDC, const RECT& rcPaint);

	public:
		void AddLabel(LPCTSTR _DataLabel);
		void AddSeries(CChartSeries* _ChartSeries);
		CChartSeries* GetSeries(LPCTSTR _LegendText);
		CChartSeries* GetSeries(int nIndex);
		CDuiString GetSeriesName(int nIndex);
		void SetVisibleSeries(LPCTSTR _LegendText,bool _Visible);
		bool GetVisibleSeries(LPCTSTR _LegendText);
		void CalLegendSize(CChartSeries* _ChartSeries );
		CChartXYAxis& GetXYAxis();
		void RemoveSeries(int _Index);
		void RemoveSeries(CChartSeries* _ChartSeries);
		void RemoveAllData();
		UINT GetGroupCount();
		UINT GetSeriesCount();
	private:
		CChartXYAxis						m_cXYAxis;
		TStdPtrArray<CChartSeries*>			m_rChartSeries;
	};
}

#endif // UIChart_h__
