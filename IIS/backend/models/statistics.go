/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package models

type WinrateStatistic struct {
	Wins       int `json:"wins"`
	Loses      int `json:"loses"`
	Percentage int `json:"percentage"`
}

type DisciplineStatistic struct {
	Name        string `json:"name"`
	Tournaments int    `json:"tournaments"`
}

type ActivityStatistic struct {
	Month    string `json:"month"`
	Personal int    `json:"personal"`
	Teams    int    `json:"teams"`
}
